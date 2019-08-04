//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
/* float4 VS(float4 Pos : POSITION) : SV_POSITION
{
    return Pos;
} */

////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VSMain(VertexInputType input)
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(output.position, world);
    output.position = mul(input.position, view);
    output.position = mul(output.position, proj);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
    return output;
}