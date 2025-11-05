//As a whole
Vec3 traceRay(const Ray& ray, int depth, int giDepth, int maxDepth, int maxGIBounces) {
if (depth >= maxDepth) return Vec3(0.0f);
HitInfo hit = intersectScene(ray);
if (!hit.hit) return backgroundColor(ray);
Vec3 color = computeDirectLighting(hit);
// Specular reflection (not counted as GI bounce)
if (hit.material.reflective) {
Vec3 reflectDir = reflect(ray.direction, hit.normal);
Ray reflectedRay(hit.point + reflectDir * 0.001f, reflectDir);
color += hit.material.reflectivity * traceRay(reflectedRay, depth + 1, giDepth, maxDepth,
maxGIBounces);
}
// GI diffuse bounce (counted)
if (hit.material.diffuse && giDepth < maxGIBounces) {
Vec3 newDir = cosineSampleHemisphere(hit.normal);
Ray newRay(hit.point + newDir * 0.001f, newDir);
Vec3 indirect = traceRay(newRay, depth + 1, giDepth + 1, maxDepth, maxGIBounces);
color += hit.material.albedo * indirect;
}
return color;
}
