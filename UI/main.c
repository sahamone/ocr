#include "main.h"
#include <stdio.h>
#include <gtk/gtk.h>

#define BUTTON_COUNT 8

typedef struct {
    GtkWidget *imageWidget;
    GtkWidget *loadingDialog;
} DetectionData;

char* displayedimage;
GtkWidget* window;
GtkWidget* mainBox;
const char* output_path_PRT = "data/post_PRT.png";

const char* buttonLabels[BUTTON_COUNT] = 
{
    "Contrast Boost",
    "Pretreatment",
    "Rotation",
    "Automatic Rotation",
    "Detection",
    "Neural Network",
    "Solver", 
    "HELP"
};

GtkWidget* imageWidget;
GtkWidget* imagePathLabel;

gboolean update_gui_after_detection(gpointer data)
{
    DetectionData *detectionData = (DetectionData*) data;
    gtk_widget_destroy(detectionData->loadingDialog);
    displayedimage = "data/post_DET.png";
    gtk_image_set_from_file(
            GTK_IMAGE(detectionData->imageWidget),
            displayedimage);
    g_print("Detection process completed successfully!\n");
    g_free(detectionData);

    return FALSE;
}

gpointer detection_thread_func(gpointer data)
{
    DetectionData *detectionData = (DetectionData*) data;
    run_detection();
    
    g_idle_add(update_gui_after_detection, detectionData);

    return NULL;
}

void quit_button() 
{
    if (displayedimage && displayedimage != output_path_PRT) {
        g_free(displayedimage);
    }
    remove(output_path_PRT);
    gtk_main_quit();
}

void image_button(GtkWidget* widget, gpointer data) 
{
    const char* filename = displayedimage;
    const char* buttonLabel = (const char*)data;    
   
    SDL_Surface* backgroundImage = NULL;

    if (!filename) {
        GtkWidget *errorDialog = gtk_message_dialog_new(GTK_WINDOW(window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "No image loaded. Please load an image first.");
        gtk_dialog_run(GTK_DIALOG(errorDialog));
        gtk_widget_destroy(errorDialog);
        return;
    }

    if (strcmp(buttonLabel, "Contrast Boost") == 0)
    {
        backgroundImage = IMG_Load(displayedimage);
        if (backgroundImage) 
        {
            run_pretreatment(&backgroundImage, 6, 0);
            displayedimage = (char *)output_path_PRT;
            gtk_image_set_from_file(GTK_IMAGE(imageWidget), displayedimage);
            SDL_FreeSurface(backgroundImage);
        }
    }
    else if (strcmp(buttonLabel, "Pretreatment") == 0) 
    {
        GtkWidget* dialog = 
            gtk_dialog_new_with_buttons("Select Treatment Level",
                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                    GTK_DIALOG_MODAL,
                    "Light", 1,
                    "Medium", 2,
                    "Heavy", 3,
                    "Cancel", 0,
                    NULL);

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        int treatmentLevel = 0;

        if (response == 1) treatmentLevel = 1;  
        else if (response == 2) treatmentLevel = 2;  
        else if (response == 3) treatmentLevel = 3;  

        gtk_widget_destroy(dialog);

        if (treatmentLevel > 0) 
        {
            backgroundImage = IMG_Load(filename);
            if (!backgroundImage) {
                backgroundImage = IMG_Load("data/post_PRT.png");
            }
            
            if (backgroundImage) 
            {
                run_pretreatment(&backgroundImage, treatmentLevel, 0);
                displayedimage = (char *)output_path_PRT;
                gtk_image_set_from_file(GTK_IMAGE(imageWidget), displayedimage);
                SDL_FreeSurface(backgroundImage);
            }
        }
    }
    
    else if (strcmp(buttonLabel, "Rotation") == 0) 
    {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
            "Enter Rotation Angle",
            GTK_WINDOW(gtk_widget_get_toplevel(widget)),
            GTK_DIALOG_MODAL,
            "Rotate", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_CANCEL,
            NULL);

        GtkWidget *contentArea = gtk_dialog_get_content_area(
                GTK_DIALOG(dialog));
        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(
                GTK_ENTRY(entry), "Enter angle in degrees");
        gtk_box_pack_start(
                GTK_BOX(contentArea), entry, TRUE, TRUE, 0);

        gtk_widget_show_all(dialog);

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));

        if (response == GTK_RESPONSE_ACCEPT) 
        {
            const char *angleText = gtk_entry_get_text(GTK_ENTRY(entry));
            if (angleText != NULL && strlen(angleText) > 0)
            {
                int angle = atoi(angleText);
                
                printf("Rotating image by %d degrees\n", angle);
                
                SDL_Surface* backgroundImage = IMG_Load(
                        displayedimage);
                if (backgroundImage) 
                {
                    run_pretreatment(
                            &backgroundImage, 4, angle);
                    
                    if (backgroundImage) 
                    {
                        printf("SDL surface saved to: %s\n",
                                output_path_PRT);
                        displayedimage = (char*)output_path_PRT;
                        gtk_image_set_from_file(
                                GTK_IMAGE(imageWidget), displayedimage);
                        g_print("Displayed image updated to: %s\n",
                                displayedimage);
                    } 
                    else 
                    {
                        g_print("Failed to save SDL surface: %s\n", 
                                SDL_GetError());
                    }

                    SDL_FreeSurface(backgroundImage);
                } 
                else 
                {
                    g_print("Failed to load image for rotation: %s\n",
                            displayedimage);
                }
            } 
            else 
            {
                g_print(
                "Invalid input or no angle provided. Rotation canceled.\n"
                );
            }
        }

        gtk_widget_destroy(dialog);
    }

    else if (strcmp(buttonLabel, "Automatic Rotation") == 0) 
    {
        SDL_Surface* backgroundImage = IMG_Load(displayedimage);
        if (backgroundImage) 
        {
            run_pretreatment(&backgroundImage, 5, 0);
                       
            if (backgroundImage) 
            {
                printf("SDL surface saved to: %s\n", output_path_PRT);

                displayedimage = (char*)output_path_PRT;
                gtk_image_set_from_file(
                        GTK_IMAGE(imageWidget), displayedimage);
                g_print("Displayed image updated to: ");
            } 
            else 
            {
                g_print("Failed to save SDL surface: %s\n", SDL_GetError());
            }

            SDL_FreeSurface(backgroundImage);
        } 
        else 
        {
            g_print("Failed to load image for automatic rotation");
        }

    }
    
    else if (strcmp(buttonLabel, "Detection") == 0) 
    {
        SDL_Surface* processedImage = IMG_Load("data/post_PRT.png");
        if (processedImage) 
        {
            GtkWidget* loadingDialog = 
                gtk_message_dialog_new(GTK_WINDOW(window),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_NONE,
                        "Please wait a few minutes...");
            
            gtk_window_set_transient_for(
                    GTK_WINDOW(loadingDialog), GTK_WINDOW(window));
            gtk_window_set_position(
                    GTK_WINDOW(loadingDialog), GTK_WIN_POS_CENTER_ON_PARENT);

            gtk_widget_show_now(loadingDialog);

            DetectionData *detectionData = g_new(DetectionData, 1);
            detectionData->imageWidget = imageWidget;
            detectionData->loadingDialog = loadingDialog;

            g_thread_new("detection_thread",
                    detection_thread_func, detectionData);
            //run_neural();
            SDL_FreeSurface(processedImage);
        }
        else 
        {
            GtkWidget *errorDialog = 
                gtk_message_dialog_new(GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Failed to load image: %s", "data/post_PRT.png");
            gtk_dialog_run(GTK_DIALOG(errorDialog));
            gtk_widget_destroy(errorDialog);
        }
    }
    else if (strcmp(buttonLabel, "Neural Network") == 0)
    {
        run_neural();
    }
    else if (strcmp(buttonLabel, "HELP") == 0) 
    {
        
        GtkWidget* dialog = 
            gtk_dialog_new_with_buttons("Select file",
                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                    GTK_DIALOG_MODAL,
                    "Grid", 1,
                    "Word", 2,
                    "Cancel", 0,
                    NULL);

        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        int treatmentLevel = 0;

        if (response == 1) treatmentLevel = 1;  
        else if (response == 2) treatmentLevel = 2;  
        else if (response == 3) treatmentLevel = 3;  

        gtk_widget_destroy(dialog);
        
        if (response == 1) treatmentLevel = 1;  
        else if (response == 2) treatmentLevel = 2;  
        else if (response == 3) treatmentLevel = 3;  

        if (treatmentLevel==1)
        {
            const char* filePath = "data/grid";  
            if (g_file_test(filePath, G_FILE_TEST_EXISTS)) 
            {
                char command[512]; 

                snprintf(command, sizeof(command), "vim %s", filePath);

                int result = system(command);
                if (result != 0)
                {
                    GtkWidget *errorDialog = 
                        gtk_message_dialog_new(GTK_WINDOW(window),
                                GTK_DIALOG_MODAL,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_OK,
                                "Failed to open AUX file: %s", filePath);
                    gtk_dialog_run(GTK_DIALOG(errorDialog));
                    gtk_widget_destroy(errorDialog);
                }
            }
            else
            {
                GtkWidget *errorDialog = 
                    gtk_message_dialog_new(GTK_WINDOW(window),
                            GTK_DIALOG_MODAL,
                            GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_OK,
                            "File does not exist: %s", filePath);
                gtk_dialog_run(GTK_DIALOG(errorDialog));
                gtk_widget_destroy(errorDialog);
            }
        }
        if (treatmentLevel==2)
        {
            const char* filePath = "data/word";  
            if (g_file_test(filePath, G_FILE_TEST_EXISTS)) 
            {
                char command[512]; 

                snprintf(command, sizeof(command), "vim %s", filePath);

                int result = system(command);
                if (result != 0)
                {
                    GtkWidget *errorDialog = 
                        gtk_message_dialog_new(GTK_WINDOW(window),
                                GTK_DIALOG_MODAL,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_OK,
                                "Failed to open AUX file: %s", filePath);
                    gtk_dialog_run(GTK_DIALOG(errorDialog));
                    gtk_widget_destroy(errorDialog);
                }
            }
            else
            {
                GtkWidget *errorDialog = 
                    gtk_message_dialog_new(GTK_WINDOW(window),
                            GTK_DIALOG_MODAL,
                            GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_OK,
                            "File does not exist: %s", filePath);
                gtk_dialog_run(GTK_DIALOG(errorDialog));
                gtk_widget_destroy(errorDialog);
            }
        }
        
    } 
    else if (strcmp(buttonLabel, "Solver") == 0) 
    {

           
            run_solver(2);
            run_draw((char*)filename);

            

         gtk_image_set_from_file(
                 GTK_IMAGE(imageWidget), "image_sauvegardee.png");
    } 
    else 
    {
        gtk_image_set_from_file(GTK_IMAGE(imageWidget), buttonLabel);
        g_print("Button clicked: %s\n", buttonLabel);
    }
}

void load_button() 
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Open File",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename)
        {
            // Verify image can be loaded before setting it
            SDL_Surface *test_surface = IMG_Load(filename);
            if (test_surface) {
                SDL_FreeSurface(test_surface);
                if (displayedimage && displayedimage != output_path_PRT) {
                    g_free(displayedimage);
                }
                displayedimage = g_strdup(filename);
                gtk_image_set_from_file(GTK_IMAGE(imageWidget), displayedimage);
                gtk_label_set_text(GTK_LABEL(imagePathLabel), displayedimage);
            } else {
                GtkWidget *errorDialog = gtk_message_dialog_new(
                    GTK_WINDOW(window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Failed to load image: %s", filename);
                gtk_dialog_run(GTK_DIALOG(errorDialog));
                gtk_widget_destroy(errorDialog);
            }
            g_free(filename);
        }
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char* argv[])
{
    remove(output_path_PRT);
    gtk_init(&argc, &argv);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "ANSN Studio OCR");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(mainBox), 10);
    gtk_container_add(GTK_CONTAINER(window), mainBox);

    GtkWidget* topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(mainBox), topBox, TRUE, TRUE, 0);

    GtkWidget* buttonBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_hexpand(buttonBox, FALSE);
    gtk_widget_set_vexpand(buttonBox, TRUE);
    gtk_box_pack_start(GTK_BOX(topBox), buttonBox, FALSE, TRUE, 0);

    for (int i = 0; i < BUTTON_COUNT; i++) 
    {
        GtkWidget* button = gtk_button_new_with_label(buttonLabels[i]);
        g_signal_connect(button, "clicked",
                G_CALLBACK(image_button), (gpointer)buttonLabels[i]);
        gtk_box_pack_start(GTK_BOX(buttonBox), button, TRUE, TRUE, 0);
    }

    GtkWidget* quitButton = gtk_button_new_with_label("Quit");
    g_signal_connect(quitButton, "clicked", 
                    G_CALLBACK(quit_button), NULL);
    gtk_box_pack_end(GTK_BOX(buttonBox), quitButton, FALSE, FALSE, 0);

    imageWidget = gtk_image_new();
    if (argc > 1)
    {
        gtk_image_set_from_file(GTK_IMAGE(imageWidget), argv[1]);
    }
    else
    {
        gtk_image_set_from_file(GTK_IMAGE(imageWidget), 
                                "resultPretreatment.png");
    }
    gtk_box_pack_start(GTK_BOX(topBox), imageWidget, TRUE, TRUE, 0);

    GtkWidget* searchBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(mainBox), searchBox, FALSE, FALSE, 0);

    imagePathLabel = gtk_label_new("Please load an image.");
    gtk_box_pack_start(GTK_BOX(searchBox), imagePathLabel, TRUE, TRUE, 0);

    GtkWidget* loadButton = gtk_button_new_with_label("Load");
    g_signal_connect(loadButton, "clicked",
                                G_CALLBACK(load_button), NULL);
    gtk_box_pack_start(GTK_BOX(searchBox), loadButton, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}