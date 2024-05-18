/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <cmath>

#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#define TPOINT(a, b)    (Triangle.points[(a)].gl_Position[(b)])


typedef struct triangle{
    OutVertex points[3];
} triangle_t;

uint32_t computeVertexID32(VertexArray const &vao, uint32_t shaderInvocation)
{
    uint32_t *indexes = (uint32_t *)vao.indexBuffer;
    return indexes[shaderInvocation];
}   

uint16_t computeVertexID16(VertexArray const &vao, uint32_t shaderInvocation)
{
    uint16_t *indexes = (uint16_t *)vao.indexBuffer;
    return indexes[shaderInvocation];
}   

uint8_t computeVertexID8(VertexArray const &vao, uint32_t shaderInvocation)
{
    uint8_t *indexes = (uint8_t *)vao.indexBuffer;
    return indexes[shaderInvocation];
}   

void getVertexID(VertexArray const&vao, InVertex *inVertex, uint32_t v)
{
    if (vao.indexBuffer == nullptr)
        {
            //non-indexed
            inVertex->gl_VertexID = v;
        } 
        else
        {   //indexed
            switch (vao.indexType)
            {
                case IndexType::UINT32: 
                    inVertex->gl_VertexID = computeVertexID32(vao, v);
                    break;
                
                case IndexType::UINT16: 
                    inVertex->gl_VertexID = computeVertexID16(vao, v);
                    break;

                case IndexType::UINT8: 
                    inVertex->gl_VertexID = computeVertexID8(vao, v);
                    break;
            }
        }
}

void setVertexAttributes(VertexAttrib const attrib[maxAttributes], InVertex *inVertex)
{
    for (uint32_t i = 0; i < maxAttributes; i++)
    {
        uint32_t ID = inVertex->gl_VertexID;
        uint64_t stride = (attrib[i].stride);
        uint64_t offset = (attrib[i].offset);
        uint8_t *buffer = (uint8_t *) attrib[i].bufferData;
        uint8_t *ptr;
        
        switch (attrib[i].type)
        {
            case AttributeType::EMPTY:
                continue;
            
            case AttributeType::FLOAT:
                ptr = buffer + offset + stride*ID;
                inVertex->attributes[i].v1 = *((float *)ptr);
                break;

            case AttributeType::VEC2:
                for (uint32_t j = 0; j < 2; j++)
                {
                    ptr = buffer + offset + stride*ID + j*4;
                    inVertex->attributes[i].v2[j] = *((float *)ptr);
                }
                break;

            case AttributeType::VEC3:
                for (uint32_t j = 0; j < 3; j++)
                {
                    ptr = buffer + offset + stride*ID + j*4;
                    inVertex->attributes[i].v3[j] = *((float *)ptr);
                }
                break;

            case AttributeType::VEC4:
                for (uint32_t j = 0; j < 4; j++)
                {
                    ptr = buffer + offset + stride*ID + j*4;
                    inVertex->attributes[i].v4[j] = *((float *)ptr);
                }
                break;
        }
    }
}

void runVertexAssembly(VertexArray const&vao, InVertex *inVertex, uint32_t v)
{
    getVertexID(vao, inVertex, v);
    setVertexAttributes(vao.vertexAttrib, inVertex);
}

void runPrimitiveAssembly(triangle_t *Triangle, VertexArray const&vao, Program const&prg, 
        uint32_t t)
{
    //Loop thorough all vertexes in a triangle
    for (uint32_t v = 0; v < 3; v++)
    {
        InVertex inVertex;
        runVertexAssembly(vao, &inVertex, t+v);
        prg.vertexShader(Triangle->points[v], inVertex, prg.uniforms);
    }
}

void perspectiveDivision(triangle_t *Triangle)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Triangle->points[i].gl_Position[j] /= Triangle->points[i].gl_Position[3];
        }
    }
}

void viewportTransformation(Frame frame, triangle_t *Triangle)
{
    for (int p = 0; p < 3; p++)
    {
        float *x = &(Triangle->points[p].gl_Position[0]);
        *x = (*x*0.5+0.5)*frame.width;
        
        float *y = &(Triangle->points[p].gl_Position[1]);
        *y = (*y*0.5+0.5)*frame.height;
    }
}

long double sideLength(double x1, double y1, double x2, double y2)
{
    long double dx = x1-x2;
    long double dy = y1-y2;
    long double length = sqrt(pow(dx, 2)+pow(dy,2));
    return length;
}

double triangleArea(double x1, double y1, double x2, double y2, double x3, double y3)
{
    double circ = 0;

    long double a = sideLength(x1,y1,x2,y2);
    long double b = sideLength(x2,y2,x3,y3);
    long double c = sideLength(x3,y3,x1,y1);

    circ = (a+b+c)/2.0;

    //Heron's formula to calculate area of triangle
    double area = sqrt(circ*(circ - a)*(circ - b)*(circ - c));
    
    return area;
}

void getBarycentrics(triangle_t Triangle, std::vector<double> &bar, double area, int x, int y)
{
    double a = (double) x + 0.5;
    double b = (double) y + 0.5;

    double a0 = triangleArea(TPOINT(1,0),TPOINT(1,1),\
                             TPOINT(2,0),TPOINT(2,1),a, b);
    double a1 = triangleArea(TPOINT(2,0),TPOINT(2,1),\
                             TPOINT(0,0),TPOINT(0,1), a, b);
    double a2 = triangleArea(TPOINT(0,0),TPOINT(0,1),\
                             TPOINT(1,0),TPOINT(1,1), a, b);
    
    bar[0] = a0/area;
    bar[1] = a1/area;
    bar[2] = a2/area;

}

void createFragment(InFragment *inFragment, triangle_t Triangle, Program prg,\
                    std::vector<double> bar, double area, int x, int y)
{

    //setting coordinates of inFragments
    inFragment->gl_FragCoord.x = x+0.5;
    inFragment->gl_FragCoord.y = y+0.5;

    //setting depth of inFragments
    inFragment->gl_FragCoord.z = TPOINT(0,2)*bar[0] + TPOINT(1,2)*bar[1] + TPOINT(2,2)*bar[2];
    
    double s = (bar[0]/TPOINT(0,3)) + (bar[1]/TPOINT(1,3)) + (bar[2]/TPOINT(2,3));

    bar[0] = (bar[0])/(TPOINT(0,3)*s);
    bar[1] = (bar[1])/(TPOINT(1,3)*s);
    bar[2] = (bar[2])/(TPOINT(2,3)*s);

    double res;

    for (int i = 0; i < maxAttributes; i++){
        if (prg.vs2fs[i] == AttributeType::VEC3)
        {
            for (int i = 0; i < 3; i++)
            {
                res = 0;
                for (int j = 0; j < 3; j++)
                {
                    res += bar[j] * Triangle.points[j].attributes[0].v3[i];
                }
            
                inFragment->attributes[0].v3[i] = res;     
            }
        } else if (prg.vs2fs[i] == AttributeType::VEC4)
        {
            for (int i = 0; i < 4; i++)
            {
                res = 0;
                for (int j = 0; j < 3; j++)
                {
                    res += bar[j] * Triangle.points[j].attributes[0].v4[i];
                }
            
                inFragment->attributes[0].v4[i] = res;     
            }
        }
    }
}

float clampColor(float c)
{
    if (c > 1)
    {
        c = 1.0;
    } 

    return c;
}

void perFragmentOperations(Frame *frame, OutFragment outFragment, int pix, float z)
{
    float r = clampColor(outFragment.gl_FragColor.r);
    float g = clampColor(outFragment.gl_FragColor.g);
    float b = clampColor(outFragment.gl_FragColor.b);
    float a = outFragment.gl_FragColor.w;

    if (a < 1)
    {
        r = clampColor((frame->color[pix*4]/255.f)*(1-a) + (r)*(a));
        g = clampColor((frame->color[(pix*4)+1]/255.f)*(1-a) + (g)*(a));
        b = clampColor((frame->color[(pix*4)+2]/255.f)*(1-a) + (b)*(a));
    }

    if (z < frame->depth[pix])
    {
        if (a > 0.5)
        {
            frame->depth[pix] = z;
        }

        frame->color[pix*4] = r*255.f;
        frame->color[(pix*4)+1] = g*255.f;
        frame->color[(pix*4)+2] = b*255.f;
        return;
    }
}

void rasterize(Frame &frame, triangle_t const&Triangle, Program const&prg)
{   
    int minX = MIN(MIN(TPOINT(0,0), TPOINT(1,0)), TPOINT(2,0));
    int minY = MIN(MIN(TPOINT(0,1), TPOINT(1,1)), TPOINT(2,1));

    int maxX = MAX(MAX(TPOINT(0,0), TPOINT(1,0)), TPOINT(2,0));
    int maxY = MAX(MAX(TPOINT(0,1), TPOINT(1,1)), TPOINT(2,1));

    minX = MAX(0, minX);
    minY = MAX(0, minY);

    maxX = MIN(frame.width - 1, maxX); 
    maxY = MIN(frame.height - 1, maxY);
    

    double deltaX1 = TPOINT(1,0) - TPOINT(0,0);
    double deltaX2 = TPOINT(2,0) - TPOINT(1,0);
    double deltaX3 = TPOINT(0,0) - TPOINT(2,0);

    double deltaY1 = TPOINT(1,1) - TPOINT(0,1);
    double deltaY2 = TPOINT(2,1) - TPOINT(1,1);
    double deltaY3 = TPOINT(0,1) - TPOINT(2,1);

    double edgeF1 = (minY - (TPOINT(0,1)-0.5))*deltaX1 - (minX - (TPOINT(0,0)-0.5))*deltaY1;
    double edgeF2 = (minY - (TPOINT(1,1)-0.5))*deltaX2 - (minX - (TPOINT(1,0)-0.5))*deltaY2;
    double edgeF3 = (minY - (TPOINT(2,1)-0.5))*deltaX3 - (minX - (TPOINT(2,0)-0.5))*deltaY3;
    
    double area = triangleArea(TPOINT(0,0),TPOINT(0,1),TPOINT(1,0),\
                                   TPOINT(1,1),TPOINT(2,0),TPOINT(2,1));

    std::vector<double> barycentrics(3);
    int pix;
    
    for (int y = minY; y <= maxY; y++) {
        bool even = ((y - minY) % 2 == 0);
        for (int x = ((even) ? minX : maxX); (even) ? (x <= maxX) : \
            (x >= minX); x += (even) ? 1 : -1) {
            if (edgeF1 >= 0 && edgeF2 >= 0 && edgeF3 >= 0) {
                InFragment inFragment;
                            
                getBarycentrics(Triangle, barycentrics, area, x, y);
                createFragment(&inFragment, Triangle, prg, barycentrics, area, x, y);
                            
                OutFragment outFragment;
                prg.fragmentShader(outFragment,inFragment,prg.uniforms);

                pix = y*frame.width+x;
                perFragmentOperations(&frame, outFragment, pix, inFragment.gl_FragCoord.z);
            }

            if (!((even && x == maxX) || (!even && x == minX))) {
                edgeF1 += (even) ? -deltaY1 : deltaY1;
                edgeF2 += (even) ? -deltaY2 : deltaY2;
                edgeF3 += (even) ? -deltaY3 : deltaY3;
            }
        }
        edgeF1 += deltaX1;
        edgeF2 += deltaX2;
        edgeF3 += deltaX3;
    }
}

//! [drawTrianglesImpl]
void drawTrianglesImpl(GPUContext &ctx, uint32_t nofVertices){
    for(uint32_t t = 0; t < nofVertices; t += 3)
    {
        triangle_t Triangle;
        runPrimitiveAssembly(&Triangle, ctx.vao, ctx.prg, t);

        perspectiveDivision(&Triangle);
        viewportTransformation(ctx.frame, &Triangle);

        rasterize(ctx.frame, Triangle, ctx.prg);
    }
}

//! [drawTrianglesImpl]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv){
  if(!texture.data)return glm::vec4(0.f);
  auto uv1 = glm::fract(uv);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  //auto t   = glm::fract(uv2);
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  for(uint32_t c=0;c<texture.channels;++c)
    color[c] = texture.data[(pix.y*texture.width+pix.x)*texture.channels+c]/255.f;
  return color;
}

/**
 * @brief This function clears framebuffer.
 *
 * @param ctx GPUContext
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void clear(GPUContext&ctx,float r,float g,float b,float a){
  auto&frame = ctx.frame;
  auto const nofPixels = frame.width * frame.height;
  for(size_t i=0;i<nofPixels;++i){
    frame.depth[i] = 10e10f;
    frame.color[i*4+0] = static_cast<uint8_t>(glm::min(r*255.f,255.f));
    frame.color[i*4+1] = static_cast<uint8_t>(glm::min(g*255.f,255.f));
    frame.color[i*4+2] = static_cast<uint8_t>(glm::min(b*255.f,255.f));
    frame.color[i*4+3] = static_cast<uint8_t>(glm::min(a*255.f,255.f));
  }
}

