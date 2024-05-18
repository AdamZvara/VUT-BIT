/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <student/drawModel.hpp>
#include <student/gpu.hpp>

 
void drawNode(GPUContext&ctx,Node const&node,Model const&model,glm::mat4 matrix){

    if(node.mesh>=0){
        Mesh const&mesh = model.meshes[node.mesh];
    
        ctx.prg.uniforms.uniform[1].m4 = matrix*node.modelMatrix;
        ctx.prg.uniforms.uniform[2].m4 = glm::transpose(glm::inverse(matrix*node.modelMatrix));
        ctx.prg.uniforms.uniform[5].v4 = mesh.diffuseColor;
    
        ctx.vao.vertexAttrib[0] = mesh.position;
        ctx.vao.vertexAttrib[1] = mesh.normal;
        ctx.vao.vertexAttrib[2] = mesh.texCoord;
            
        if(mesh.diffuseTexture>=0){
            ctx.prg.uniforms.textures[0] = model.textures[mesh.diffuseTexture];
            ctx.prg.uniforms.uniform[6].v1 = 1.f; 
        } else {
            ctx.prg.uniforms.textures[0] = Texture{};
            ctx.prg.uniforms.uniform[6].v1 = 0.f; 
        }

        drawTriangles(ctx,mesh.nofIndices);
    }
 
    for(size_t i=0;i<node.children.size();++i){
        drawNode(ctx, node.children[i], model, matrix*node.modelMatrix);
    }

}
/**
 * @brief This function renders a model
 *
 * @param ctx GPUContext
 * @param model model structure
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position (unused)
 */
//! [drawModel]
void drawModel(GPUContext&ctx,Model const&model,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
    ctx.prg.vertexShader = drawModel_vertexShader;
    ctx.prg.fragmentShader = drawModel_fragmentShader;

    ctx.prg.vs2fs[0] = AttributeType::VEC3;
    ctx.prg.vs2fs[1] = AttributeType::VEC3;
    ctx.prg.vs2fs[2] = AttributeType::VEC2;

    ctx.prg.uniforms.uniform[0].m4 = proj*view;
    ctx.prg.uniforms.uniform[3].v3 = light;


    glm::mat4 identityM = glm::mat4(1.f);
    for(size_t i=0;i<model.roots.size();++i){
        drawNode(ctx, model.roots[i], model, identityM);
    }
}
//! [drawModel]

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [drawModel_vs]
void drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
    auto pos        = inVertex.attributes[0].v3;
    auto normal     = inVertex.attributes[1].v3;
    auto texCoords  = inVertex.attributes[2].v2;

    auto projMatrix     = uniforms.uniform[0].m4;
    auto modelMatrix    = uniforms.uniform[1].m4;
    auto inverseMatrix  = uniforms.uniform[2].m4;
    
    outVertex.gl_Position      = projMatrix * modelMatrix * glm::vec4(pos,1.f);
    outVertex.attributes[0].v3 = modelMatrix*glm::vec4(pos,1.f); 
    outVertex.attributes[1].v3 = inverseMatrix*glm::vec4(normal,0.f);
    outVertex.attributes[2].v2 = texCoords;
}
//! [drawModel_vs]

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
//! [drawModel_fs]
void drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
    auto light          = uniforms.uniform[3].v3;
    auto diffuseColor   = uniforms.uniform[5].v4;
    auto tex            = uniforms.uniform[6].v1;
    auto diffuseTexture = uniforms.uniform[0].v4;

    auto pos       = inFragment.attributes[0].v3;
    auto nor       = inFragment.attributes[1].v3;
    auto texCoords = inFragment.attributes[2].v2;

    auto N=glm::normalize(nor);
    
    if (tex > 0){
        diffuseColor = read_texture(uniforms.textures[0], texCoords);
    }


    auto L = glm::normalize(light-pos);
    float dF = glm::max(glm::dot(L,glm::normalize(nor)),0.f);
    float aF = 0.2f;

    glm::vec3 dL = glm::vec3(diffuseColor)*dF;
    glm::vec3 aL = glm::vec3(diffuseColor)*aF;
   
    outFragment.gl_FragColor = glm::vec4(glm::vec3(aL+dL), diffuseColor.a);
}
//! [drawModel_fs]

