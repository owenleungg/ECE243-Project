#include "globals.h"

void build_gaussian_kernal() {
    int kernal_radius = (int)KERNAL_SIZE / 2;
    
    double sum = 0.0;
    for (int i = -kernal_radius; i <= kernal_radius; i++)
    {
        for (int j = -kernal_radius; j <= kernal_radius; j++)
        {
            double val = (1.0 / (2.0 * M_PI * pow(SIGMA, 2))) * exp(-((pow(i, 2) + pow(j, 2)) / (2.0 * pow(SIGMA, 2))));
            gaussian_kernal[i + kernal_radius][j + kernal_radius] = val;
            sum += val;
        }
    }
    printf("Kernel sum: %f\n", sum);

    // Normalize the kernel
    for (int i = 0; i < KERNAL_SIZE; i++)
    {
        for (int j = 0; j < KERNAL_SIZE; j++)
        {
            gaussian_kernal[i][j] /= sum;
        }
    }
}

void apply_grey_scale() {

    // Convert input_frame to grey scale
    for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT; i++)
    {
        short int pixel = input_frame[i];

        // Extract RGB components
        short int r5 = (pixel >> 11) & 0x1F;
        short int g6 = (pixel >> 5) & 0x3F;
        short int b5 = pixel & 0x1F;

        // Convert to 8-bit values
        short int r = (r5 * 255) / 31;
        short int g = (g6 * 255) / 63;
        short int b = (b5 * 255) / 31;

        // Calculate grayscale
        short int gray = (short int)(0.3 * r + 0.59 * g + 0.11 * b);

        // Convert 8-bit gray back to RGB565
        short int gray_r5 = (short int)((gray / 255.0) * 31 + 0.5);  // Round to nearest
        short int gray_g6 = (short int)((gray / 255.0) * 63 + 0.5);
        short int gray_b5 = (short int)((gray / 255.0) * 31 + 0.5);

        // Repack to 16-bit RGB565
        grayscale[i] = (gray_r5 << 11) | (gray_g6 << 5) | gray_b5;
    }
}

void apply_gaussian_kernal() {

    int kernal_radius = (int)KERNAL_SIZE / 2;

    // Convolve input_frame with kernal
    for (int x = kernal_radius; x < IMAGE_WIDTH - kernal_radius; x++) {
        for (int y = kernal_radius; y < IMAGE_HEIGHT - kernal_radius; y++) {
            double accum = 0.0;
    
            for (int ky = -kernal_radius; ky <= kernal_radius; ky++) {
                for (int kx = -kernal_radius; kx <= kernal_radius; kx++) {
                    int img_x = x + kx;
                    int img_y = y + ky;
    
                    short int pixel = grayscale[img_y * IMAGE_WIDTH + img_x];
    
                    // Extract any one component (green is most sensitive)
                    short int g6 = (pixel >> 5) & 0x3F;
                    double intensity = (double)(g6 * 255) / 63;

                    double weight = gaussian_kernal[ky + kernal_radius][kx + kernal_radius];
                    accum += intensity * weight;
                }
            }
    
            // Convert back to RGB565
            short int final_intensity = (short int)accum;
            short int r5 = (final_intensity * 31) / 255;
            short int g6 = (final_intensity * 63) / 255;
            short int b5 = (final_intensity * 31) / 255;

            blurred[y * IMAGE_WIDTH + x] = (r5 << 11) | (g6 << 5) | b5;
        }
    }
}

void apply_sobel_operator() {
    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    
    int Gy[3][3] = {
        { 1,  2,  1},
        { 0,  0,  0},
        {-1, -2, -1}
    };
    
    for (int x = 1; x < IMAGE_WIDTH - 1; x++) {
        for (int y = 1; y < IMAGE_HEIGHT - 1; y++) {
            int gx = 0;
            int gy = 0;
    
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int img_x = x + kx;
                    int img_y = y + ky;
    
                    short int pixel = blurred[img_y * IMAGE_WIDTH + img_x];
    
                    // Extract any one component (green is most sensitive)
                    short int g6 = (pixel >> 5) & 0x3F;
                    int intensity = (g6 * 255) / 63;
    
                    gx += intensity * Gx[ky + 1][kx + 1];
                    gy += intensity * Gy[ky + 1][kx + 1];
                }
            }
    
            int gradient_mag = sqrt(pow(gx, 2) + pow(gy, 2));
            if (gradient_mag > 255) gradient_mag = 255;

            // Convert back to RGB565 grayscale
            short int r5 = (gradient_mag * 31) / 255;
            short int g6 = (gradient_mag * 63) / 255;
            short int b5 = (gradient_mag * 31) / 255;
            gradient[y * IMAGE_WIDTH + x] = (r5 << 11) | (g6 << 5) | b5;
        }
    }
}

void apply_non_max_suppression() {
    for (int x = 1; x < IMAGE_WIDTH - 1; x++) {
        for (int y = 1; y < IMAGE_HEIGHT - 1; y++) {
            int gx = gx_buffer[y * IMAGE_WIDTH + x];
            int gy = gy_buffer[y * IMAGE_WIDTH + x];
    
            float angle = atan2(gy, gx) * 180.0 / M_PI;
            if (angle < 0) angle += 180;
    
            int current = gradient[y * IMAGE_WIDTH + x];
            int neighbor1 = 0, neighbor2 = 0;
    
            // Directional suppression
            // Compares current to neighbors
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
                // East and west neighbors
                neighbor1 = gradient[y * IMAGE_WIDTH + (x + 1)];
                neighbor2 = gradient[y * IMAGE_WIDTH + (x - 1)];
            } else if (angle >= 22.5 && angle < 67.5) {
                // Northeast and southwest neighbors
                neighbor1 = gradient[(y + 1) * IMAGE_WIDTH + (x - 1)];
                neighbor2 = gradient[(y - 1) * IMAGE_WIDTH + (x + 1)];
            } else if (angle >= 67.5 && angle < 112.5) {
                // North and south neighbors
                neighbor1 = gradient[(y + 1) * IMAGE_WIDTH + x];
                neighbor2 = gradient[(y - 1) * IMAGE_WIDTH + x];
            } else if (angle >= 112.5 && angle < 157.5) {
                // Northwest and southeast neighbors
                neighbor1 = gradient[(y - 1) * IMAGE_WIDTH + (x - 1)];
                neighbor2 = gradient[(y + 1) * IMAGE_WIDTH + (x + 1)];
            }
    
            if (current >= neighbor1 && current >= neighbor2) {
                // Keep pixel
                suppressed[y * IMAGE_WIDTH + x] = current;
            } else {
                // Suppress
                suppressed[y * IMAGE_WIDTH + x] = 0;
            }
        }
    }
    
}

void apply_double_threshold() {

    for (int x = 1; x < IMAGE_WIDTH - 1; x++) {
        for (int y = 1; y < IMAGE_HEIGHT - 1; y++) {
            uint16_t pixel = gradient[y * IMAGE_WIDTH + x];

            // Extract any one component (green is most sensitive)
            short int g6 = (pixel >> 5) & 0x3F;
            double intensity = (double)(g6) / 63; // Normalized to 0-1

            if (intensity > HIGH_THRESHOLD) {
                thresholded[y * IMAGE_WIDTH + x] = gradient[y * IMAGE_WIDTH + x];
                thresholds[y * IMAGE_WIDTH + x] = STRONG_EDGE;
            } else if (intensity > LOW_THRESHOLD) {
                thresholded[y * IMAGE_WIDTH + x] = gradient[y * IMAGE_WIDTH + x];
                thresholds[y * IMAGE_WIDTH + x] = WEAK_EDGE;
            } else {
                thresholded[y * IMAGE_WIDTH + x] = 0;
                thresholds[y * IMAGE_WIDTH + x] = 0;
            }
        }
    }   
}

void apply_edge_tracking() {
    for (int x = 1; x < IMAGE_WIDTH - 1; x++) {
        for (int y = 1; y < IMAGE_HEIGHT - 1; y++) {
            
            if (thresholds[y * IMAGE_WIDTH + x] == STRONG_EDGE) {
                // Strong edge
                hysteresis[y * IMAGE_WIDTH + x] = STRONG_EDGE;
                continue;
            }

            if (thresholds[y * IMAGE_WIDTH + x] == 0) {
                // Not Strong or weak edge
                hysteresis[y * IMAGE_WIDTH + x] = 0;
                continue;
            }

            if (thresholds[y * IMAGE_WIDTH + x] == WEAK_EDGE) {
                // Weak edge
                bool connected_to_strong = false;

                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
  
                        if (thresholds[(y + dy) * IMAGE_WIDTH + (x + dx)] == STRONG_EDGE) {
                            connected_to_strong = true;
                        }
                    }
                }

                if (connected_to_strong) {
                    hysteresis[y * IMAGE_WIDTH + x] = STRONG_EDGE;
                } else {
                    // Not connected to strong edge so suppress
                    hysteresis[y * IMAGE_WIDTH + x] = 0;
                }
            
            }
        }
    }
}