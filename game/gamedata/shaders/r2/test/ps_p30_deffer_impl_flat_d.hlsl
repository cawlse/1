//
// Generated by Microsoft (R) D3DX9 Shader Compiler 9.07.239.0000
//
//   fxc /DUSE_SHADER3=1 /nologo /Tps_3_0 /Emain /Zpr
//    /Fctest\p30_deffer_impl_flat_d.ps deffer_impl_flat_d.ps
//
//
// Parameters:
//
//   float4 L_material;
//   sampler2D s_base;
//   sampler2D s_dn_a;
//   sampler2D s_dn_b;
//   sampler2D s_dn_g;
//   sampler2D s_dn_r;
//   sampler2D s_dt_a;
//   sampler2D s_dt_b;
//   sampler2D s_dt_g;
//   sampler2D s_dt_r;
//   sampler2D s_mask;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   L_material   c0       1
//   s_base       s0       1
//   s_mask       s1       1
//   s_dt_r       s2       1
//   s_dt_g       s3       1
//   s_dt_b       s4       1
//   s_dt_a       s5       1
//   s_dn_r       s6       1
//   s_dn_g       s7       1
//   s_dn_b       s8       1
//   s_dn_a       s9       1
//

    ps_3_0
    def c1, 1, -0.5, 0.5, 0.0250000004
    def c2, 0.00784313772, 0, 0, 0
    dcl_texcoord v1.xy
    dcl_texcoord1 v2.xyz
    dcl_texcoord2_pp v3.xyz
    dcl_texcoord3_pp v4.xyz
    dcl_texcoord4_pp v5.xyz
    dcl_texcoord5 v6.xy
    dcl_2d s0
    dcl_2d s1
    dcl_2d s2
    dcl_2d s3
    dcl_2d s4
    dcl_2d s5
    dcl_2d s6
    dcl_2d s7
    dcl_2d s8
    dcl_2d s9
    texld_pp r0, v6, s5
    texld_pp r2, v6, s4
    texld_pp r3, v6, s2
    texld_pp r4, v6, s3
    texld_pp r1, v1, s1
    dp4_pp r0.w, r1, c1.x
    rcp_pp r0.w, r0.w
    mul_pp r1, r1, r0.w
    mul_pp r4.xyz, r4, r1.y
    mad_pp r3.xyz, r3, r1.x, r4
    mad_pp r2.xyz, r2, r1.z, r3
    mad_pp r4.xyz, r0, r1.w, r2
    texld_pp r0, v1, s0
    texld_pp r2, v6, s6
    texld_pp r3, v6, s7
    add_pp r3.xyz, r3.wzyw, c1.y
    add_pp r2.xyz, r2.wzyw, c1.y
    mul_pp r3.xyz, r1.y, r3
    mad_pp r3.xyz, r2, r1.x, r3
    texld_pp r2, v6, s8
    add_pp r2.xyz, r2.wzyw, c1.y
    mad_pp r3.xyz, r2, r1.z, r3
    texld_pp r2, v6, s9
    add_pp r1.xyz, r2.wzyw, c1.y
    mul_pp r2.xyz, r4, r0
    mad_pp r0.xyz, r1, r1.w, r3
    mov_pp oC1.w, r0.w
    mul_pp r0.w, r0.z, c1.z
    dp3_pp r1.x, v3, r0.xyww
    mul_pp r0.xyz, r0, c1.xxzw
    dp3_pp r1.y, v4, r0
    dp3_pp r1.z, v5, r0
    add_pp oC2.xyz, r2, r2
    nrm_pp r0.xyz, r1
    mad_pp oC0.xyz, r0, c1.w, v2
    mov_pp oC1.xyz, r0
    mov_pp oC0.w, c0.w
    mov_pp oC2.w, c2.x

// approximately 40 instruction slots used (10 texture, 30 arithmetic)