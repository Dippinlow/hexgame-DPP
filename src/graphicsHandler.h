#pragma once

#include <string>
#include <cstdint>
#include "CImgConfig.h"
#include "renderData.h"

using namespace cimg_library;

class GraphicsHandler
{
public:
    GraphicsHandler();

    std::string render(const RenderData& rd, const std::string& c1_hex, 
                        const std::string& c2_hex, const std::string& n1, const std::string& n2, 
                        int move_count);
    std::string render_gameplay_gif(uint64_t channel_id);

private:
    void hex_to_rgb(const std::string& hex, unsigned char rgb[3]);

    void index_to_centre(float& px, float& py, int i, int j, float s);

    void draw_hexagon(CImg<unsigned char>& img, float cx, float cy, float s, const unsigned char colour[3]);
    void draw_highlighted_hexagon(CImg<unsigned char>& img, float cx, float cy, float s, const unsigned char colour[3]);

    void draw_half_hexagon(CImg<unsigned char>& img, float cx, float cy, float s,
                                         float* verts_x, float* verts_y, int count, const unsigned char colour[3], float opacity = 1.0f);

    void draw_board(CImg<unsigned char>& img, const RenderData& rd,
                    const unsigned char colour1[3], const unsigned char colour2[3]);

    void draw_edges(CImg<unsigned char>& img, const RenderData& rd,
                    unsigned char colour1[3], unsigned char colour2[3]);

    void draw_centred_text(CImg<unsigned char>& img, const std::string& txt, float cx, float cy, unsigned char colour[3], int s);

    void dim_colour(unsigned char* col, unsigned char* out, float factor);

    void draw_edge_labels(CImg<unsigned char>& img);

    void draw_banner_left(CImg<unsigned char>& img, const std::string& name, unsigned char colour[3]);
    void draw_banner_right(CImg<unsigned char>& img, const std::string& name, unsigned char colour[3]);

    unsigned char get_average_brightness(unsigned char colour[3]);

};