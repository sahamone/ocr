#include "../include/canny.h"

#define PI M_PI

void sobel_filter(custIMG *img, float **gradient_magnitude, float **gradient_direction)
{
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}};
    int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}};

    for (unsigned int y = 1; y < img->height - 1; y++)
    {
        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            int gx_r = 0, gy_r = 0;
            int gx_g = 0, gy_g = 0;
            int gx_b = 0, gy_b = 0;

            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    gx_r += img->pixels[y + ky][x + kx].r * Gx[ky + 1][kx + 1];
                    gy_r += img->pixels[y + ky][x + kx].r * Gy[ky + 1][kx + 1];

                    gx_g += img->pixels[y + ky][x + kx].g * Gx[ky + 1][kx + 1];
                    gy_g += img->pixels[y + ky][x + kx].g * Gy[ky + 1][kx + 1];

                    gx_b += img->pixels[y + ky][x + kx].b * Gx[ky + 1][kx + 1];
                    gy_b += img->pixels[y + ky][x + kx].b * Gy[ky + 1][kx + 1];
                }
            }

            float grad_r = sqrt(gx_r * gx_r + gy_r * gy_r);
            float grad_g = sqrt(gx_g * gx_g + gy_g * gy_g);
            float grad_b = sqrt(gx_b * gx_b + gy_b * gy_b);

            gradient_magnitude[y][x] = (grad_r + grad_g + grad_b) / 3.0;

            gradient_direction[y][x] = atan2(gy_r + gy_g + gy_b, gx_r + gx_g + gx_b) * 180 / PI;
        }
    }
}

void nm_filter(custIMG *img, float **gradient_magnitude, float **gradient_direction, float **edges)
{
    for (unsigned int y = 1; y < img->height - 1; y++)
    {
        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            float direction = gradient_direction[y][x];
            float magnitude = gradient_magnitude[y][x];
            float mag1, mag2;

            if ((direction >= -22.5 && direction <= 22.5) || (direction >= 157.5 || direction <= -157.5))
            {
                mag1 = gradient_magnitude[y][x - 1];
                mag2 = gradient_magnitude[y][x + 1];
            }
            else if ((direction > 22.5 && direction <= 67.5) || (direction < -112.5 && direction >= -157.5))
            {
                mag1 = gradient_magnitude[y - 1][x + 1];
                mag2 = gradient_magnitude[y + 1][x - 1];
            }
            else if ((direction > 67.5 && direction <= 112.5) || (direction < -67.5 && direction >= -112.5))
            {
                mag1 = gradient_magnitude[y - 1][x];
                mag2 = gradient_magnitude[y + 1][x];
            }
            else
            {
                mag1 = gradient_magnitude[y - 1][x - 1];
                mag2 = gradient_magnitude[y + 1][x + 1];
            }

            if (magnitude >= mag1 && magnitude >= mag2)
            {
                edges[y][x] = magnitude;
            }
            else
            {
                edges[y][x] = 0;
            }
        }
    }
}

void dilate_filter(unsigned char **input, unsigned char **output, unsigned int height, unsigned int width)
{
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            output[y][x] = 0;
        }
    }

    for (unsigned int y = 1; y < height - 1; y++)
    {
        for (unsigned int x = 1; x < width - 1; x++)
        {
            if (input[y][x] == 1)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        output[y + dy][x + dx] = 1;
                    }
                }
            }
        }
    }
}

void hyst_aux(unsigned char **edge_map, unsigned int y, unsigned int x, unsigned int height, unsigned int width)
{
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dy == 0 && dx == 0)
                continue;
            int ny = y + dy;
            int nx = x + dx;
            if (ny >= 0 && (unsigned int)ny < height && nx >= 0 && (unsigned int)nx < width)
            {
                if (edge_map[ny][nx] == 1)
                {
                    edge_map[ny][nx] = 2;
                    hyst_aux(edge_map, ny, nx, height, width);
                }
            }
        }
}

void hysteresis_filter(custIMG *img, float **edges, float low_thresh, float high_thresh, unsigned char **edge_map)
{
    for (unsigned int y = 0; y < img->height; y++)
        for (unsigned int x = 0; x < img->width; x++)
            edge_map[y][x] = 0;

    for (unsigned int y = 1; y < img->height - 1; y++)

        for (unsigned int x = 1; x < img->width - 1; x++)
        {
            if (edges[y][x] >= high_thresh)
                edge_map[y][x] = 2;
            else if (edges[y][x] >= low_thresh)
                edge_map[y][x] = 1;
            else
                edge_map[y][x] = 0;
        }

    for (unsigned int y = 1; y < img->height - 1; y++)
        for (unsigned int x = 1; x < img->width - 1; x++)
            if (edge_map[y][x] == 2)
                hyst_aux(edge_map, y, x, img->height, img->width);

    for (unsigned int y = 0; y < img->height; y++)
        for (unsigned int x = 0; x < img->width; x++)
            if (edge_map[y][x] == 1)
                edge_map[y][x] = 0;
            else if (edge_map[y][x] == 2)
                edge_map[y][x] = 1;
}

void process(custIMG *img)
{
    float **gradient_magnitude = (float **)malloc(img->height * sizeof(float *));
    float **gradient_direction = (float **)malloc(img->height * sizeof(float *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        gradient_magnitude[i] = (float *)malloc(img->width * sizeof(float));
        gradient_direction[i] = (float *)malloc(img->width * sizeof(float));
    }
    sobel_filter(img, gradient_magnitude, gradient_direction);

    float **edges = (float **)malloc(img->height * sizeof(float *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        edges[i] = (float *)malloc(img->width * sizeof(float));
    }
    nm_filter(img, gradient_magnitude, gradient_direction, edges);

    unsigned char **edge_map = (unsigned char **)malloc(img->height * sizeof(unsigned char *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        edge_map[i] = (unsigned char *)malloc(img->width * sizeof(unsigned char));
    }
    float low_thresh = 20.0;
    float high_thresh = 50.0;
    hysteresis_filter(img, edges, low_thresh, high_thresh, edge_map);

    unsigned char **dilated_edge_map = (unsigned char **)malloc(img->height * sizeof(unsigned char *));
    for (unsigned int i = 0; i < img->height; i++)
    {
        dilated_edge_map[i] = (unsigned char *)malloc(img->width * sizeof(unsigned char));
    }
    dilate_filter(edge_map, dilated_edge_map, img->height, img->width);

    BoundingBox *boxes;
    int num_boxes;
    find_bounding_boxes(dilated_edge_map, img->height, img->width, &boxes, &num_boxes);


    Color green = {0, 255, 0};
    for (int i = 0; i < num_boxes; i++)
    {
        int width = boxes[i].max_x - boxes[i].min_x;
        int height = boxes[i].max_y - boxes[i].min_y;
        if (width > 5 && height > 5)
        {
            draw_rectangle(img, boxes[i].min_x, boxes[i].min_y, boxes[i].max_x, boxes[i].max_y, green, i);
        }
    }

    for (unsigned int i = 0; i < img->height; i++)
    {
        free(gradient_magnitude[i]);
        free(gradient_direction[i]);
        free(edges[i]);
        free(edge_map[i]);
        free(dilated_edge_map[i]);
    }
    free(gradient_magnitude);
    free(gradient_direction);
    free(edges);
    free(edge_map);
    free(dilated_edge_map);
    free(boxes);
}