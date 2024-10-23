struct FragmentInput {
    @location(0) tex_coord: vec2<f32>,
};

@group(0) @binding(0) var texture_sampler: sampler;
@group(0) @binding(1) var texture: texture_2d<f32>;

@fragment
fn main(input: FragmentInput) -> @location(0) vec4<f32> {
    return textureSample(texture, texture_sampler, input.tex_coord);
}