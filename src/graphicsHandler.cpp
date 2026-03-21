#include "graphicsHandler.h"

static const int ANTI_ALIASING = 4; 

static const float HEX_SIZE = 22.0f * ANTI_ALIASING;
static const float TEXT_SIZE = 20 * ANTI_ALIASING;

static const int IMG_WIDTH = 800;
static const int IMG_HEIGHT = 450;

static float SQRT3 = sqrtf(3.0f);
static float SQRT3_2= sqrtf(3.0f) / 2.0f;

static float HEX_VERTS_X[6] = {1.0f, 0.5f, -0.5f, -1.0f, -0.5f, 0.5f};
static float HEX_VERTS_Y[6];// = {0, SQRT3_2, SQRT3_2, 0, -SQRT3_2, -SQRT3_2};

static float OFFSET_X = 0.0f;
static float OFFSET_Y = 0.0f;

static float BANNER_HEI = IMG_HEIGHT * ANTI_ALIASING / 5.0f;
static float BANNER_LEN1 = IMG_WIDTH * ANTI_ALIASING / 6.0f;
static float BANNER_LEN2 =  (BANNER_HEI * 3.0f / SQRT3) + BANNER_LEN1;


static unsigned char BACKGROUND1[3] = {40, 40, 41};
static unsigned char BACKGROUND2[3] = {80, 80, 81};
static unsigned char WHITE[3] = {255, 255, 255};
static unsigned char BLACK[3] = {0, 0, 0};


// insanity
static float HEX_HALF_TOP_X[4];
static float HEX_HALF_TOP_Y[4];
static float HEX_HALF_BOT_X[4];
static float HEX_HALF_BOT_Y[4];
static float HEX_HALF_RIGHT_X[5];
static float HEX_HALF_RIGHT_Y[5];
static float HEX_HALF_LEFT_X[5];
static float HEX_HALF_LEFT_Y[5];

GraphicsHandler::GraphicsHandler()
{
    HEX_VERTS_Y[0] =  0.0f;
    HEX_VERTS_Y[1] =  SQRT3_2;
    HEX_VERTS_Y[2] =  SQRT3_2;
    HEX_VERTS_Y[3] =  0.0f;
    HEX_VERTS_Y[4] = -SQRT3_2;
    HEX_VERTS_Y[5] = -SQRT3_2;

    HEX_HALF_TOP_X[0] =  1.0f;  HEX_HALF_TOP_Y[0] =  0.0f;
    HEX_HALF_TOP_X[1] =  0.5f;  HEX_HALF_TOP_Y[1] = SQRT3_2;
    HEX_HALF_TOP_X[2] = -0.5f;  HEX_HALF_TOP_Y[2] = SQRT3_2;
    HEX_HALF_TOP_X[3] = -1.0f;  HEX_HALF_TOP_Y[3] =  0.0f;

    HEX_HALF_BOT_X[0] =  1.0f;  HEX_HALF_BOT_Y[0] =  0.0f;
    HEX_HALF_BOT_X[1] =  0.5f;  HEX_HALF_BOT_Y[1] =  -SQRT3_2;
    HEX_HALF_BOT_X[2] = -0.5f;  HEX_HALF_BOT_Y[2] =  -SQRT3_2;
    HEX_HALF_BOT_X[3] = -1.0f;  HEX_HALF_BOT_Y[3] =  0.0f;

    HEX_HALF_RIGHT_X[0] =  1.0f;  HEX_HALF_RIGHT_Y[0] = 0.0f;
    HEX_HALF_RIGHT_X[1] =  0.5f;  HEX_HALF_RIGHT_Y[1] =  SQRT3_2;
    HEX_HALF_RIGHT_X[2] =  0.0f;  HEX_HALF_RIGHT_Y[2] =  SQRT3_2;
    HEX_HALF_RIGHT_X[3] =  0.0f;  HEX_HALF_RIGHT_Y[3] =  -SQRT3_2;
    HEX_HALF_RIGHT_X[4] =  0.5f;  HEX_HALF_RIGHT_Y[4] = -SQRT3_2;

    HEX_HALF_LEFT_X[0] =  -1.0f;  HEX_HALF_LEFT_Y[0] = 0.0f;
    HEX_HALF_LEFT_X[1] = -0.5f;  HEX_HALF_LEFT_Y[1] = SQRT3_2;
    HEX_HALF_LEFT_X[2] =  0.0f;  HEX_HALF_LEFT_Y[2] =  SQRT3_2;
    HEX_HALF_LEFT_X[3] = 0.0f;  HEX_HALF_LEFT_Y[3] =  -SQRT3_2;
    HEX_HALF_LEFT_X[4] =  -0.5f;  HEX_HALF_LEFT_Y[4] =  -SQRT3_2;

    float cx, cy;
    index_to_centre(cx, cy, 5, 5, HEX_SIZE);

    OFFSET_X += (IMG_WIDTH  * ANTI_ALIASING / 2.0f) - cx;
    OFFSET_Y += (IMG_HEIGHT * ANTI_ALIASING / 2.0f) - cy;   
}

void GraphicsHandler::dim_colour(unsigned char* col, unsigned char* out, float factor)
{
    out[0] = (unsigned char)(col[0] * factor + BACKGROUND1[0] * (1.0f - factor));
    out[1] = (unsigned char)(col[1] * factor + BACKGROUND1[1] * (1.0f - factor));
    out[2] = (unsigned char)(col[2] * factor + BACKGROUND1[2] * (1.0f - factor));
}


std::string GraphicsHandler::render(const RenderData& rd, const std::string& c1_hex, const std::string& c2_hex, 
                                    const std::string& n1, const std::string& n2)
{
    unsigned char colour1[3], colour2[3];
    hex_to_rgb(c1_hex, colour1);
    hex_to_rgb(c2_hex, colour2);

    //CImg<unsigned char> img(IMG_WIDTH, IMG_HEIGHT, 1, 3, 10);
    CImg<unsigned char> img(IMG_WIDTH * ANTI_ALIASING, IMG_HEIGHT * ANTI_ALIASING, 1, 3, 20);

    unsigned char colour1_dimmed[3], colour2_dimmed[3];
    float factor1 = (rd.turn == 0) ? 1.0f : 0.3f;
    float factor2 = (rd.turn == 1) ? 1.0f : 0.3f;

    dim_colour(colour1, colour1_dimmed, factor1);
    dim_colour(colour2, colour2_dimmed, factor2);

    draw_edges(img, rd, colour1_dimmed, colour2_dimmed);
    draw_board(img, rd, colour1, colour2);
    draw_edge_labels(img);
    //draw_indicator(img, rd, colour1, colour2);

    std::string left_name  = rd.axis_swapped ? n2 : n1;
    std::string right_name = rd.axis_swapped ? n1 : n2;
    unsigned char* left_col  = rd.axis_swapped ? colour2 : colour1;
    unsigned char* right_col = rd.axis_swapped ? colour1 : colour2;

draw_banner_left(img,  left_name,  left_col);
draw_banner_right(img, right_name, right_col);

    img = img.resize(IMG_WIDTH, IMG_HEIGHT, 1, 3, 6);
    std::string path = "../temp/board_" + std::to_string(rd.channel_id) + ".png";
    img.save_png(path.c_str());
    return path;
}

void GraphicsHandler::hex_to_rgb(const std::string& hex, unsigned char rgb[3])
{
    std::string h = hex;
    if (h[0] == '#') h = h.substr(1);

    rgb[0] = (unsigned char)std::stoi(h.substr(0, 2), nullptr, 16);
    rgb[1] = (unsigned char)std::stoi(h.substr(2, 2), nullptr, 16);
    rgb[2] = (unsigned char)std::stoi(h.substr(4, 2), nullptr, 16);
}

void GraphicsHandler::draw_centred_text(CImg<unsigned char>& img, const std::string& txt, float cx, float cy, unsigned char colour[3], int s)
{
    int text_w = txt.size() * (s * 0.6f);
    int text_h = s;

    img.draw_text(cx - text_w / 2.0f, cy - text_h / 2.0f, txt.c_str(), colour, 0, 1, s);
}

void GraphicsHandler::index_to_centre(float& px, float& py, int i, int j, float s)
{
    px = OFFSET_X + (i + j) * 1.5f * s;
    py = OFFSET_Y + (i - j) * (SQRT3 / 2.0f) * s;
}

void GraphicsHandler::draw_hexagon(CImg<unsigned char>& img, float cx, float cy, float s, const unsigned char c[3]) 
{
    CImg<float> points(6, 2); 

     for(int i = 0; i < 6; i++)
     {
          points(i,0) = s * HEX_VERTS_X[i] + cx;
          points(i,1) = s * HEX_VERTS_Y[i] + cy;
     }
     img.draw_polygon(points, c);
}

void GraphicsHandler::draw_highlighted_hexagon(CImg<unsigned char>& img, float cx, float cy, float s, const unsigned char c[3]) 
{
    unsigned char complement[3];
    if((c[0]+c[1]+c[2]) / 3 > 128)
    {
        complement[0] = (unsigned char)std::max(0, (int)c[0] - 80);
        complement[1] = (unsigned char)std::max(0, (int)c[1] - 80);
        complement[2] = (unsigned char)std::max(0, (int)c[2] - 80);
    }
    else 
    {
        complement[0] = (unsigned char)std::min(255, (int)c[0] + 80);
        complement[1] = (unsigned char)std::min(255, (int)c[1] + 80);
        complement[2] = (unsigned char)std::min(255, (int)c[2] + 80);
    }
    draw_hexagon(img, cx, cy, s*1.1f, complement);
    draw_hexagon(img, cx, cy, s*0.9f, c);
}

void GraphicsHandler::draw_half_hexagon(CImg<unsigned char>& img, float cx, float cy, float s,
                                         float* verts_x, float* verts_y, int count, const unsigned char colour[3], float o)
{
    CImg<float> points(count, 2);
    for (int i = 0; i < count; i++)
    {
        points(i, 0) = cx + s * verts_x[i];
        points(i, 1) = cy + s * verts_y[i];
    }
    img.draw_polygon(points, colour, o);
}

void GraphicsHandler::draw_board(CImg<unsigned char>& img, const RenderData& rd, const unsigned char colour1[3], const unsigned char colour2[3])
{
    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            float cx, cy;
            index_to_centre(cx, cy, i, j, HEX_SIZE);

            const unsigned char* col = BACKGROUND2;
            if (rd.board[i][j] == 1) col = colour1;
            else if (rd.board[i][j] == 2) col = colour2;

            
            if (rd.board[i][j] != 0)
                draw_hexagon(img, cx, cy, HEX_SIZE, col);
            else
                draw_hexagon(img, cx, cy, HEX_SIZE * 0.9f, col);
        }
    }

    if (rd.last_i != -1 && rd.last_j != -1)
    {
        float cx, cy;
        index_to_centre(cx, cy, rd.last_i, rd.last_j, HEX_SIZE);

        const unsigned char* col = BACKGROUND2;
        if (rd.board[rd.last_i][rd.last_j] == 1) col = colour1;
        else if (rd.board[rd.last_i][rd.last_j] == 2) col = colour2;
        draw_highlighted_hexagon(img, cx, cy, HEX_SIZE, col);
    }
}

void GraphicsHandler::draw_edges(CImg<unsigned char>& img, const RenderData& rd, unsigned char colour1[3], unsigned char colour2[3]) 
{
    unsigned char* i_axis_col = rd.axis_swapped ? colour2 : colour1;
    unsigned char* j_axis_col = rd.axis_swapped ? colour1 : colour2;

    float edge_s = HEX_SIZE * 1.2f;

    for (int k = 1; k < 10; k++)
    {
        float cx, cy;

        index_to_centre(cx, cy, 0,  k, HEX_SIZE);
        draw_hexagon(img, cx, cy, edge_s, i_axis_col);

        index_to_centre(cx, cy, 10, k, HEX_SIZE);
        draw_hexagon(img, cx, cy, edge_s, i_axis_col);

        index_to_centre(cx, cy, k, 0,  HEX_SIZE);
        draw_hexagon(img, cx, cy, edge_s, j_axis_col);

        index_to_centre(cx, cy, k, 10, HEX_SIZE);
        draw_hexagon(img, cx, cy, edge_s, j_axis_col);
    }

    float cx, cy;

    index_to_centre(cx, cy, 0, 0, HEX_SIZE);
    draw_half_hexagon(img, cx, cy, edge_s, HEX_HALF_TOP_X, HEX_HALF_TOP_Y, 4, j_axis_col);
    draw_half_hexagon(img, cx, cy, edge_s, HEX_HALF_BOT_X, HEX_HALF_BOT_Y, 4, i_axis_col);

    index_to_centre(cx, cy, 10, 10, HEX_SIZE);
    draw_half_hexagon(img, cx, cy, edge_s, HEX_HALF_TOP_X, HEX_HALF_TOP_Y, 4, i_axis_col);
    draw_half_hexagon(img, cx, cy, edge_s, HEX_HALF_BOT_X, HEX_HALF_BOT_Y, 4, j_axis_col);

    index_to_centre(cx, cy, 0, 10, HEX_SIZE);
    draw_half_hexagon(img, cx, cy, edge_s, HEX_HALF_LEFT_X,  HEX_HALF_LEFT_Y,  5, i_axis_col);
    draw_half_hexagon(img, cx, cy, edge_s, HEX_HALF_RIGHT_X, HEX_HALF_RIGHT_Y, 5, j_axis_col);

    index_to_centre(cx, cy, 10, 0, HEX_SIZE);
    draw_half_hexagon(img, cx, cy, edge_s, HEX_HALF_RIGHT_X, HEX_HALF_RIGHT_Y, 5, i_axis_col);
    draw_half_hexagon(img, cx, cy, edge_s, HEX_HALF_LEFT_X,  HEX_HALF_LEFT_Y,  5, j_axis_col);

    for (int i = 0; i < 11; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            float cx, cy;
            index_to_centre(cx, cy, i, j, HEX_SIZE);
            draw_hexagon(img, cx, cy, HEX_SIZE, BACKGROUND1);
        }
    }
}

void GraphicsHandler::draw_edge_labels(CImg<unsigned char>& img)
{
    unsigned char gray[3] = {150, 150, 150};
    float dx = 2.0f * HEX_SIZE * 0.75f;
    float dy = 2.0f * HEX_SIZE * (SQRT3 / 4.0f);

    for (int i = 0; i < 11; i++)
    {
        std::string label(1, 'A' + i);
        float cx, cy;

        index_to_centre(cx, cy, i, 0, HEX_SIZE);
        draw_centred_text(img, label, cx - dx, cy + dy, gray, TEXT_SIZE);

        index_to_centre(cx, cy, i, 10, HEX_SIZE);
        draw_centred_text(img, label, cx + dx, cy - dy, gray, TEXT_SIZE);
    }

    for (int j = 0; j < 11; j++)
    {
        std::string label = std::to_string(j + 1);
        float cx, cy;

        index_to_centre(cx, cy, 0, j, HEX_SIZE);
        draw_centred_text(img, label, cx - dx, cy - dy, gray, TEXT_SIZE);

        index_to_centre(cx, cy, 10, j, HEX_SIZE);
        draw_centred_text(img, label, cx + dx, cy + dy, gray, TEXT_SIZE);
    }
}

void GraphicsHandler::draw_banner_left(CImg<unsigned char>& img, const std::string& name, unsigned char colour[3])
{
    CImg<float> points(4, 2);
    points(0, 0) = 0;               points(0, 1) = 0;
    points(1, 0) = BANNER_LEN2;     points(1, 1) = 0;
    points(2, 0) = BANNER_LEN1;     points(2, 1) = BANNER_HEI;
    points(3, 0) = 0;               points(3, 1) = BANNER_HEI;
    
    img.draw_polygon(points, colour);

    unsigned char* text_col = WHITE;
    if((colour[0] + colour[1] + colour[2]) / 3 > 128) text_col = BLACK;

    img.draw_text(BANNER_LEN1*0.1f, BANNER_HEI * 0.1f, name.c_str(), text_col, 0, 1, 48 * ANTI_ALIASING);
}

void GraphicsHandler::draw_banner_right(CImg<unsigned char>& img, const std::string& name, unsigned char colour[3])
{
    float w = IMG_WIDTH  * ANTI_ALIASING;

    CImg<float> points(4, 2);
    points(0, 0) = w;                 points(0, 1) = 0;
    points(1, 0) = w;                 points(1, 1) = BANNER_HEI;
    points(2, 0) = w - BANNER_LEN1;   points(2, 1) = BANNER_HEI;
    points(3, 0) = w - BANNER_LEN2;   points(3, 1) = 0;

    img.draw_polygon(points, colour);

    unsigned char* text_col = WHITE;
    if((colour[0] + colour[1] + colour[2]) / 3 > 128) text_col = BLACK;

    img.draw_text(w - BANNER_LEN2 * 0.65f, BANNER_HEI * 0.1f, name.c_str(), text_col, 0, 1, 48 * ANTI_ALIASING);
}

