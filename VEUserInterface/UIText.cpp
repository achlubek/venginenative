#include "stdafx.h"
#include "UIText.h"
#include "UIBitmap.h"
#include "UIRenderer.h"


int uitext_fsize(FILE* fh) {
    int prev = ftell(fh);
    fseek(fh, 0L, SEEK_END);
    int sz = ftell(fh);
    fseek(fh, prev, SEEK_SET);
    return sz;
}

int uitext_get_file_contents_binary(unsigned char** out_bytes, const char* path) {
    FILE* fh = fopen(path, "rb");
    int size = uitext_fsize(fh);
    unsigned char* content = (unsigned char*)calloc(size, 1);
    size_t sz = fread(content, 1, size, fh);
    fclose(fh);
    *out_bytes = content;
    return size;
}

UIText::UIText(UIRenderer* irenderer, float ix, float iy, UIColor icolor, std::string fontpath, int ifontsize, std::string text)
    : renderer(irenderer), x(ix), y(iy), color(icolor), fontsize(ifontsize)
{
    uitext_get_file_contents_binary(&fontBuffer, fontpath.c_str());
    font = new stbtt_fontinfo();
    if (!stbtt_InitFont(font, fontBuffer, 0))
    {
        printf("failed\n");
    }
    dataBuffer = new VulkanGenericBuffer(irenderer->vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1024);
    set = renderer->layout->generateDescriptorSet();
    updateText(text);
}


UIText::~UIText()
{
}

void UIText::updateBuffer()
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    bb.emplaceFloat32(x);
    bb.emplaceFloat32(y);
    bb.emplaceFloat32(width);
    bb.emplaceFloat32(height);

    bb.emplaceFloat32(color.red);
    bb.emplaceFloat32(color.green);
    bb.emplaceFloat32(color.blue);
    bb.emplaceFloat32(color.alpha);

    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32(1.0f);
    bb.emplaceFloat32(1.0f);

    bb.emplaceInt32(3);
    bb.emplaceInt32(3);
    bb.emplaceInt32(3);
    bb.emplaceInt32(3);

    void* data;
    dataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    dataBuffer->unmap();
}

void UIText::updateText(std::string text)
{ 
    int l_h = fontsize;  
     
    float scale = stbtt_ScaleForPixelHeight(font, l_h);

    const char* word = text.c_str();

    int nx = 0;
    int maxy = 0;

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);

    ascent *= scale;
    descent *= scale;

    for (int i = 0; i < strlen(word); ++i)
    {
        /* get bounding box for character (may be offset to account for chars that dip above or below the line */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(font, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* compute y (different characters have different heights */
        int y = (c_y2 - c_y1) + ascent;
        if (y > maxy) maxy = y;

        /* render character (stride and offset is important here) */
       // int byteOffset = x + (y  * b_w);
       // stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);

        /* how wide is this character */
        int ax;
        stbtt_GetCodepointHMetrics(font, word[i], &ax, 0);
        nx += ax * scale;

        /* add kerning */
        int kern;
        kern = stbtt_GetCodepointKernAdvance(font, word[i], word[i + 1]);
        nx += kern * scale;
        nx += 2;
    }
    unsigned char* bitmap = new unsigned char[nx * maxy];
    for (int i = 0; i < nx * maxy; i++)bitmap[i] = 0;
    int b_w = nx;

    nx = 0;
    for (int i = 0; i < strlen(word); ++i)
    {
        /* get bounding box for character (may be offset to account for chars that dip above or below the line */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(font, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* compute y (different characters have different heights */
        int y = ascent + c_y1;

        /* render character (stride and offset is important here) */
        int byteOffset = nx + (y  * b_w);
        stbtt_MakeCodepointBitmap(font, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);

        /* how wide is this character */
        int ax;
        stbtt_GetCodepointHMetrics(font, word[i], &ax, 0);
        nx += ax * scale;

        /* add kerning */
        int kern;
        kern = stbtt_GetCodepointKernAdvance(font, word[i], word[i + 1]);
        nx += kern * scale;
        nx += 2;
    }

   // free(fontBuffer);
   // free(bitmap);

    ImageData img = ImageData();
    img.width = nx;
    img.height = maxy;
    img.channelCount = 1;
    img.data = (void*)bitmap;

    width = (float)nx / (float)renderer->width;
    height = (float)maxy / (float)renderer->height; 

    if (texture != nullptr) delete texture;
    texture = renderer->vulkan->createTexture(img, VK_FORMAT_R8_UNORM);
    set->bindUniformBuffer(0, dataBuffer);
    set->bindImageViewSampler(1, texture);
    set->update();

   // delete[] bitmap; // stb deletes
}
