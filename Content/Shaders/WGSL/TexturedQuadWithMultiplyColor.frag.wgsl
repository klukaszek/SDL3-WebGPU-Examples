struct FragmentInput {
    @location(0) tex_coord: vec2<f32>,
};

struct UniformBlock {
    multiply_color: vec4<f32>,
};

@group(2) @binding(0) var texture_sampler: sampler;
@group(2) @binding(1) var texture: texture_2d<f32>;
@group(3) @binding(0) var<uniform> uniforms: UniformBlock;

@fragment
fn main(input: FragmentInput) -> @location(0) vec4<f32> {
    return uniforms.multiply_color * textureSample(texture, texture_sampler, input.tex_coord);
}
