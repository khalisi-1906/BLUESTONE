// Sphere distance function
float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

// Raymarch a single sphere
float rayMarchSphere(vec3 ro, vec3 rd, vec3 center, float radius) {
    float t = 0.0;
    for (int i = 0; i < 100; i++) {
        vec3 p = ro + rd * t;
        float d = sdSphere(p, center, radius);
        if (d < 0.001) return t;
        t += d;
        if (t > 20.0) break;
    }
    return -1.0;
}

// Calculate normal
vec3 calcNormal(vec3 p, vec3 center, float radius) {
    const float eps = 0.001;
    vec2 e = vec2(eps, 0);
    float d = sdSphere(p, center, radius);
    return normalize(vec3(
        sdSphere(p + e.xyy, center, radius) - d,
        sdSphere(p + e.yxy, center, radius) - d,
        sdSphere(p + e.yyx, center, radius) - d
    ));
}

// Get sphere color with texture mapping
vec4 getSphereColor(vec3 p, vec3 normal, vec3 baseColor, float opacity, sampler2D tex) {
    vec3 texColor = texture(tex, normal.xy * 0.5 + 0.5).rgb;
    vec3 color = mix(texColor, baseColor, 0.5);
    return vec4(color, opacity);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = (fragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
   
    // Camera setup
    vec3 ro = vec3(0.0, 0.0, 3.0);
    vec3 rd = normalize(vec3(uv, -1.0));
   
    // Sphere parameters
    float anim = sin(iTime * 0.5) * 0.1;
    vec3 center1 = vec3(-0.3 + anim, 0.0, 0.0);
    vec3 center2 = vec3(0.3 - anim, 0.0, 0.0);
    float radius = 0.8;
   
    // Raymarch both spheres
    float dist1 = rayMarchSphere(ro, rd, center1, radius);
    float dist2 = rayMarchSphere(ro, rd, center2, radius);
   
    // Background color
    fragColor = texture(iChannel0, uv * 0.5 + 0.5);
   
    if (dist1 > 0.0 || dist2 > 0.0) {
        // Determine which sphere is closer
        bool sphere1Closer = (dist1 > 0.0) && ((dist2 < 0.0) || (dist1 < dist2));
       
        if (sphere1Closer) {
            // Render opaque red sphere first
            vec3 p1 = ro + rd * dist1;
            vec3 n1 = calcNormal(p1, center1, radius);
            vec4 col1 = getSphereColor(p1, n1, vec3(1.0, 0.2, 0.2), 1.0, iChannel0);
           
            // Then render transparent blue sphere behind it
            if (dist2 > 0.0) {
                vec3 p2 = ro + rd * dist2;
                vec3 n2 = calcNormal(p2, center2, radius);
                vec4 col2 = getSphereColor(p2, n2, vec3(0.2, 0.2, 1.0), 0.6, iChannel0);
                fragColor = mix(fragColor, col2, col2.a);
            }
           
            fragColor = mix(fragColor, col1, col1.a);
        } else {
            // Render transparent blue sphere first
            if (dist2 > 0.0) {
                vec3 p2 = ro + rd * dist2;
                vec3 n2 = calcNormal(p2, center2, radius);
                vec4 col2 = getSphereColor(p2, n2, vec3(0.2, 0.2, 1.0), 0.6, iChannel0);
                fragColor = mix(fragColor, col2, col2.a);
            }
           
            // Then render opaque red sphere behind it
            if (dist1 > 0.0) {
                vec3 p1 = ro + rd * dist1;
                vec3 n1 = calcNormal(p1, center1, radius);
                vec4 col1 = getSphereColor(p1, n1, vec3(1.0, 0.2, 0.2), 1.0, iChannel0);
                fragColor = mix(fragColor, col1, col1.a);
            }
        }
       
        s
       
        // Special handling for intersection area
        if (dist1 > 0.0 && dist2 > 0.0 && abs(dist1 - dist2) < 0.05) {
            vec3 p = ro + rd * min(dist1, dist2);
            vec3 n1 = calcNormal(p, center1, radius);
            vec3 n2 = calcNormal(p, center2, radius);
            vec3 n = normalize(n1 + n2);
            fragColor.rgb = mix(fragColor.rgb, vec3(1.0, 0.5, 1.0), 0.3);
            fragColor.rgb += vec3(0.3) * pow(max(0.0, dot(n, normalize(vec3(1.0)))), 4.0);
        }
    }
}
