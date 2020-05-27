#include "utility.h"

#include <cslang/Parser.h>
#include <cslang/Statement.h>
#include <cslang/GenCode.h>

#include <catch/catch.hpp>

#include <sstream>
#include <iostream>

TEST_CASE("global_var")
{
    auto str = R"(

int val;
uniform vec2 val2;

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
int val;

uniform vec2 val2;

)"+1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

TEST_CASE("ellipse")
{
    auto str = R"(

float ellipse(vec2 uv, float width, float height)
{
    float d = length((uv * 2 - 1) / vec2(width, height));
    return clamp((1 - d) / fwidth(d), 0.0, 1.0);
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
float ellipse(vec2 uv, float width, float height)
{
    float(d) = length((((uv * 2) - 1) / vec2(width, height)));
    return clamp(((1 - d) / fwidth(d)), 0.0, 1.0);
}

)"+1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

TEST_CASE("hue")
{
    auto str = R"(

vec3 hue(vec3 rgb, float offset)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 P = mix(vec4(rgb.bg, K.wz), vec4(rgb.gb, K.xy), step(rgb.b, rgb.g));
    vec4 Q = mix(vec4(P.xyw, rgb.r), vec4(rgb.r, P.yzx), step(P.x, rgb.r));
    float D = Q.x - min(Q.w, Q.y);
    float E = 1e-10;
    vec3 hsv = vec3(abs(Q.z + (Q.w - Q.y)/(6.0 * D + E)), D / (Q.x + E), Q.x);

    float hue = hsv.x + offset;
    hsv.x = (hue < 0)
            ? hue + 1
            : (hue > 1)
                ? hue - 1
                : hue;

    // HSV to RGB
    vec4 K2 = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 P2 = abs(fract(hsv.xxx + K2.xyz) * 6.0 - K2.www);
    return hsv.z * mix(K2.xxx, clamp(P2 - K2.xxx, 0.0, 1.0), hsv.y);
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
vec3 hue(vec3 rgb, float offset)
{
    vec4(K) = vec4(0.0, ((- 1.0) / 3.0), (2.0 / 3.0), (- 1.0));
    vec4(P) = mix(vec4(rgb.bg, K.wz), vec4(rgb.gb, K.xy), step(rgb.b, rgb.g));
    vec4(Q) = mix(vec4(P.xyw, rgb.r), vec4(rgb.r, P.yzx), step(P.x, rgb.r));
    float(D) = (Q.x - min(Q.w, Q.y));
    float(E) = 1e-10.0;
    vec3(hsv) = vec3(abs((Q.z + ((Q.w - Q.y) / ((6.0 * D) + E)))), (D / (Q.x + E)), Q.x);
    float(hue) = (hsv.x + offset);
    hsv.x = ((hue < 0) ? (hue + 1) : ((hue > 1) ? (hue - 1) : hue));
    vec4(K2) = vec4(1.0, (2.0 / 3.0), (1.0 / 3.0), 3.0);
    vec3(P2) = abs(((fract((hsv.xxx + K2.xyz)) * 6.0) - K2.www));
    return (hsv.z * mix(K2.xxx, clamp((P2 - K2.xxx), 0.0, 1.0), hsv.y));
}

)"+1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

TEST_CASE("white_balance")
{
    auto str = R"(

// https://docs.unity3d.com/Packages/com.unity.shadergraph@9.0/manual/White-Balance-Node.html
vec3 white_balance(vec3 rgb, float temperature, float tint)
{
    // Range ~[-1.67;1.67] works best
    float t1 = temperature * 10 / 6;
    float t2 = tint * 10 / 6;

    // Get the CIE xy chromaticity of the reference white point.
    // Note: 0.31271 = x value on the D65 white point
    float x = 0.31271 - t1 * (t1 < 0 ? 0.1 : 0.05);
    float standardIlluminantY = 2.87 * x - 3 * x * x - 0.27509507;
    float y = standardIlluminantY + t2 * 0.05;

    // Calculate the coefficients in the LMS space.
    vec3 w1 = vec3(0.949237, 1.03542, 1.08728); // D65 white point

    // CIExyToLMS
    float Y = 1;
    float X = Y * x / y;
    float Z = Y * (1 - x - y) / y;
    float L = 0.7328 * X + 0.4296 * Y - 0.1624 * Z;
    float M = -0.7036 * X + 1.6975 * Y + 0.0061 * Z;
    float S = 0.0030 * X + 0.0136 * Y + 0.9834 * Z;
    vec3 w2 = vec3(L, M, S);

    vec3 balance = vec3(w1.x / w2.x, w1.y / w2.y, w1.z / w2.z);

    mat3 LIN_2_LMS_MAT = mat3(
        3.90405e-1, 5.49941e-1, 8.92632e-3,
        7.08416e-2, 9.63172e-1, 1.35775e-3,
        2.31082e-2, 1.28021e-1, 9.36245e-1
    );

    mat3 LMS_2_LIN_MAT = mat3(
        2.85847e+0, -1.62879e+0, -2.48910e-2,
        -2.10182e-1,  1.15820e+0,  3.24281e-4,
        -4.18120e-2, -1.18169e-1,  1.06867e+0
    );

    vec3 lms = LIN_2_LMS_MAT * rgb;
    lms *= balance;
    return LMS_2_LIN_MAT * lms;
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
vec3 white_balance(vec3 rgb, float temperature, float tint)
{
    float(t1) = ((temperature * 10) / 6);
    float(t2) = ((tint * 10) / 6);
    float(x) = (0.31271 - (t1 * ((t1 < 0) ? 0.1 : 0.05)));
    float(standardIlluminantY) = (((2.87 * x) - ((3 * x) * x)) - 0.275095);
    float(y) = (standardIlluminantY + (t2 * 0.05));
    vec3(w1) = vec3(0.949237, 1.03542, 1.08728);
    float(Y) = 1;
    float(X) = ((Y * x) / y);
    float(Z) = ((Y * ((1 - x) - y)) / y);
    float(L) = (((0.7328 * X) + (0.4296 * Y)) - (0.1624 * Z));
    float(M) = ((((- 0.7036) * X) + (1.6975 * Y)) + (0.0061 * Z));
    float(S) = (((0.003 * X) + (0.0136 * Y)) + (0.9834 * Z));
    vec3(w2) = vec3(L, M, S);
    vec3(balance) = vec3((w1.x / w2.x), (w1.y / w2.y), (w1.z / w2.z));
    mat3(LIN_2_LMS_MAT) = mat3(0.390405, 0.549941, 0.00892632, 0.0708416, 0.963172, 0.00135775, 0.0231082, 0.128021, 0.936245);
    mat3(LMS_2_LIN_MAT) = mat3(2.85847, (- 1.62879), (- 0.024891), (- 0.210182), 1.1582, 0.000324281, (- 0.041812), (- 0.118169), 1.06867);
    vec3(lms) = (LIN_2_LMS_MAT * rgb);
    lms *= balance;
    return (LMS_2_LIN_MAT * lms);
}

)"+1);

    //cslang::GenTranslationUnit(std::cout, stat);
}


TEST_CASE("rounded_polygon")
{
    auto str = R"(

// proposed solution from
// http://stackoverflow.com/questions/26070410/robust-atany-x-on-glsl-for-converting-xy-coordinate-to-angle
// swaps params when |x| <= |y|
float atan2(float y, float x)
{
    bool s = (abs(x) > abs(y));
    return mix(3.14159265358979/2.0 - atan(x,y), atan(y,x), s);
}

// https://stackoverflow.com/questions/7610631/glsl-mod-vs-hlsl-fmod
float fmod(float x, float y)
{
    //return x ¨C y * trunc(x/y);
    return x - y * floor(x/y);
}

// https://docs.unity3d.com/Packages/com.unity.shadergraph@9.0/manual/Rounded-Polygon-Node.html
/////////////////////////////////////////////////
/// @export rounded_polygon
/////////////////////////////////////////////////
float rounded_polygon(vec2 uv, float width, float height, float sides, float roundness)
{
    const float PI = 3.14159265359;
    const float HALF_PI = 1.570796327;

    uv = uv * 2. + vec2(-1.,-1.);
    // fixme: parser
    //float epsilon = 1e-6;
    //uv.x = uv.x / ( width + (width==0?1:0)*epsilon);
    //uv.y = uv.y / ( height + (height==0?1:0)*epsilon);
    //roundness = clamp(roundness, 1e-6, 1.);
    float epsilon = 0;
    uv.x = uv.x / ( width + ((width==0)?1:0)*epsilon);
    uv.y = uv.y / ( height + ((height==0)?1:0)*epsilon);
    roundness = clamp(roundness, 0., 1.);
    float i_sides = floor( abs( sides ) );
    float fullAngle = 2. * PI / i_sides;
    float halfAngle = fullAngle / 2.;
    float opositeAngle = HALF_PI - halfAngle;
    float diagonal = 1. / cos( halfAngle );
    // Chamfer values
    float chamferAngle = roundness * halfAngle; // Angle taken by the chamfer
    float remainingAngle = halfAngle - chamferAngle; // Angle that remains
    float ratio = tan(remainingAngle) / tan(halfAngle); // This is the ratio between the length of the polygon's triangle and the distance of the chamfer center to the polygon center
    // Center of the chamfer arc
    vec2 chamferCenter = vec2(
        cos(halfAngle) ,
        sin(halfAngle)
    )* ratio * diagonal;
    // starting of the chamfer arc
    vec2 chamferOrigin = vec2(
        1.,
        tan(remainingAngle)
    );
    // Using Al Kashi algebra, we determine:
    // The distance distance of the center of the chamfer to the center of the polygon (side A)
    float distA = length(chamferCenter);
    // The radius of the chamfer (side B)
    float distB = 1. - chamferCenter.x;
    // The refence length of side C, which is the distance to the chamfer start
    float distCref = length(chamferOrigin);
    // This will rescale the chamfered polygon to fit the uv space
    // diagonal = length(chamferCenter) + distB;
    float uvScale = diagonal;
    uv *= uvScale;
    vec2 polaruv = vec2 (
        atan2( uv.y, uv.x ),
        length(uv)
    );
    polaruv.x += HALF_PI + 2*PI;
    polaruv.x = fmod( polaruv.x + halfAngle, fullAngle );
    polaruv.x = abs(polaruv.x - halfAngle);
    uv = vec2( cos(polaruv.x), sin(polaruv.x) ) * polaruv.y;
    // Calculate the angle needed for the Al Kashi algebra
    float angleRatio = 1. - (polaruv.x-remainingAngle) / chamferAngle;
    // Calculate the distance of the polygon center to the chamfer extremity
    float distC = sqrt( distA*distA + distB*distB - 2.*distA*distB*cos( PI - halfAngle * angleRatio ) );
    float ret = uv.x;
    // fixme: parser
    //float chamferZone = ( halfAngle - polaruv.x ) < chamferAngle ? 1 : 0;
    float chamferZone = (( halfAngle - polaruv.x ) < chamferAngle) ? 1 : 0;
    ret = mix( uv.x, polaruv.y / distC, chamferZone );
    // Output this to have the shape mask instead of the distance field
    ret = clamp((1 - ret) / fwidth(ret), 0.0, 1.0);
    return ret;
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
float atan2(float y, float x)
{
    bool(s) = (abs(x) > abs(y));
    return mix(((3.14159 / 2.0) - atan(x, y)), atan(y, x), s);
}

float fmod(float x, float y)
{
    return (x - (y * floor((x / y))));
}

float rounded_polygon(vec2 uv, float width, float height, float sides, float roundness)
{
    float(PI) = 3.14159;
    float(HALF_PI) = 1.5708;
    uv = ((uv * 2.0) + vec2((- 1.0), (- 1.0)));
    float(epsilon) = 0;
    uv.x = (uv.x / (width + (((width == 0) ? 1 : 0) * epsilon)));
    uv.y = (uv.y / (height + (((height == 0) ? 1 : 0) * epsilon)));
    roundness = clamp(roundness, 0.0, 1.0);
    float(i_sides) = floor(abs(sides));
    float(fullAngle) = ((2.0 * PI) / i_sides);
    float(halfAngle) = (fullAngle / 2.0);
    float(opositeAngle) = (HALF_PI - halfAngle);
    float(diagonal) = (1.0 / cos(halfAngle));
    float(chamferAngle) = (roundness * halfAngle);
    float(remainingAngle) = (halfAngle - chamferAngle);
    float(ratio) = (tan(remainingAngle) / tan(halfAngle));
    vec2(chamferCenter) = ((vec2(cos(halfAngle), sin(halfAngle)) * ratio) * diagonal);
    vec2(chamferOrigin) = vec2(1.0, tan(remainingAngle));
    float(distA) = length(chamferCenter);
    float(distB) = (1.0 - chamferCenter.x);
    float(distCref) = length(chamferOrigin);
    float(uvScale) = diagonal;
    uv *= uvScale;
    vec2(polaruv) = vec2(atan2(uv.y, uv.x), length(uv));
    polaruv.x += (HALF_PI + (2 * PI));
    polaruv.x = fmod((polaruv.x + halfAngle), fullAngle);
    polaruv.x = abs((polaruv.x - halfAngle));
    uv = (vec2(cos(polaruv.x), sin(polaruv.x)) * polaruv.y);
    float(angleRatio) = (1.0 - ((polaruv.x - remainingAngle) / chamferAngle));
    float(distC) = sqrt((((distA * distA) + (distB * distB)) - (((2.0 * distA) * distB) * cos((PI - (halfAngle * angleRatio))))));
    float(ret) = uv.x;
    float(chamferZone) = (((halfAngle - polaruv.x) < chamferAngle) ? 1 : 0);
    ret = mix(uv.x, (polaruv.y / distC), chamferZone);
    ret = clamp(((1 - ret) / fwidth(ret)), 0.0, 1.0);
    return ret;
}

)" + 1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

TEST_CASE("worley_noise")
{
    auto str = R"(

/// @default 0
uniform float u_seed;

/// @enum Euclideanm, Manhattan, Chebyshev
/// @default 0
uniform int u_dist_op;

/// @enum F1, F2, F1_ADD_F2, F2_SUB_F1, F1_MUL_F2, F1_DIV_F2
/// @default 0
uniform int u_features_mode;

/// @region 0.01, 0.5
/// @default 0.5
uniform float u_manhattan_scale;

uniform vec2 u_poisson_random_offset;

vec2 _random2(vec2 p)
{
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

float _euclidean_dist(vec2 diff)
{
    return diff.x * diff.x + diff.y * diff.y;
}

float _manhattan_dist(vec2 diff)
{
    return u_manhattan_scale * (abs(diff.x) + abs(diff.y));
}

float _chebyshev_dist(vec2 diff)
{
    float dx = abs(diff.x);
    float dy = abs(diff.y);
    return max(dx, dy);
}

/////////////////////////////////////////////////
/// @export worley_cells
/////////////////////////////////////////////////
float worley_cells(vec2 st)
{
    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float f1 = 1., f2 = 1.;

    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 neighbor = vec2(float(x),float(y));

            vec2 point = _random2(i_st + neighbor);
            point = 0.5 + 0.5*sin(u_seed + 6.2831 * point);

            vec2 diff = neighbor + point - f_st;
            float dist = 0;
            if (u_dist_op == 0) {
                dist = _euclidean_dist(diff);
            } else if (u_dist_op == 1) {
                dist = _manhattan_dist(diff);
            } else if (u_dist_op == 2) {
                dist = _chebyshev_dist(diff);
            }

            if (dist < f1) {
                f2 = f1;
                f1 = dist;
            } else if (dist > f1 && dist < f2) {
                f2 = dist;
            }
        }
    }

    if (u_features_mode == 0) {
        return f1;
    } else if (u_features_mode == 1) {
        return f2;
    } else if (u_features_mode == 2) {
        return f1 + f2;
    } else if (u_features_mode == 3) {
        return f2 - f1;
    } else if (u_features_mode == 4) {
        return f1 * f2;
    } else if (u_features_mode == 5) {
        return f1 / f2;
    } else {
        return 0;
    }

    //// debug draw
    //float color = 0;
    //color += min_dist;                                  // Draw the min distance (distance field)
    //color += 1.-step(.001, min_dist);                   // Draw cell center
    //color += step(.98, f_st.x) + step(.98, f_st.y);     // Draw grid
    //return color;
}

/////////////////////////////////////////////////
/// @export worley_no_cells
/// @uniform  u_poisson_random_offset vec2
/// @function poisson_number          (float)->uint
/// @function poisson_uniform_0_1     ()->float
/// @function poisson_morton          (uint, uint)->uint
/// @function poisson_seeding         (uint)->void
/////////////////////////////////////////////////
float worley_no_cells(vec2 st, float frequency)
{
    uint s = poisson_morton(uint(u_poisson_random_offset.x), uint(u_poisson_random_offset.y));
    if (s == 0u) {
        s = 1u;
    }
    poisson_seeding(s);

    float min_dist = 1;
    const float density = 1.5;
    uint num = poisson_number(frequency * frequency * density);
    for (uint i = 0u; i < num; ++i)
    {
        float x = poisson_uniform_0_1();
        float y = poisson_uniform_0_1();
        vec2 diff = vec2(x, y) * frequency - st;

        float dist = 0;
        if (u_dist_op == 0) {
            dist = _euclidean_dist(diff);
        } else if (u_dist_op == 1) {
            dist = _manhattan_dist(diff);
        } else if (u_dist_op == 2) {
            dist = _chebyshev_dist(diff);
        }
        min_dist = min(min_dist, dist);
    }

    // debug draw
    min_dist += 1.-step(.002, min_dist);   // Draw cell center

    return min_dist;
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
uniform float u_seed;

uniform int u_dist_op;

uniform int u_features_mode;

uniform float u_manhattan_scale;

uniform vec2 u_poisson_random_offset;

vec2 _random2(vec2 p)
{
    return fract((sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5));
}

float _euclidean_dist(vec2 diff)
{
    return ((diff.x * diff.x) + (diff.y * diff.y));
}

float _manhattan_dist(vec2 diff)
{
    return (u_manhattan_scale * (abs(diff.x) + abs(diff.y)));
}

float _chebyshev_dist(vec2 diff)
{
    float(dx) = abs(diff.x);
    float(dy) = abs(diff.y);
    return max(dx, dy);
}

float worley_cells(vec2 st)
{
    vec2(i_st) = floor(st);
    vec2(f_st) = fract(st);
    float(f1) = 1.0, f2 = 1.0;
    for (int(y) = (- 1); (y <= 1); y++)
    {
        for (int(x) = (- 1); (x <= 1); x++)
        {
            vec2(neighbor) = vec2(float(x), float(y));
            vec2(point) = _random2((i_st + neighbor));
            point = (0.5 + (0.5 * sin((u_seed + (6.2831 * point)))));
            vec2(diff) = ((neighbor + point) - f_st);
            float(dist) = 0;
            if ((u_dist_op == 0)) {
                dist = _euclidean_dist(diff);
            } else if ((u_dist_op == 1)) {
                dist = _manhattan_dist(diff);
            } else if ((u_dist_op == 2)) {
                dist = _chebyshev_dist(diff);
            }
            if ((dist < f1)) {
                f2 = f1;
                f1 = dist;
            } else if (((dist > f1) && (dist < f2))) {
                f2 = dist;
            }
        }
    }
    if ((u_features_mode == 0)) {
        return f1;
    } else if ((u_features_mode == 1)) {
        return f2;
    } else if ((u_features_mode == 2)) {
        return (f1 + f2);
    } else if ((u_features_mode == 3)) {
        return (f2 - f1);
    } else if ((u_features_mode == 4)) {
        return (f1 * f2);
    } else if ((u_features_mode == 5)) {
        return (f1 / f2);
    } else {
        return 0;
    }
}

float worley_no_cells(vec2 st, float frequency)
{
    uint(s) = poisson_morton(uint(u_poisson_random_offset.x), uint(u_poisson_random_offset.y));
    if ((s == 0)) {
        s = 1;
    }
    poisson_seeding(s);
    float(min_dist) = 1;
    float(density) = 1.5;
    uint(num) = poisson_number(((frequency * frequency) * density));
    for (uint(i) = 0; (i < num); (++ i))
    {
        float(x) = poisson_uniform_0_1();
        float(y) = poisson_uniform_0_1();
        vec2(diff) = ((vec2(x, y) * frequency) - st);
        float(dist) = 0;
        if ((u_dist_op == 0)) {
            dist = _euclidean_dist(diff);
        } else if ((u_dist_op == 1)) {
            dist = _manhattan_dist(diff);
        } else if ((u_dist_op == 2)) {
            dist = _chebyshev_dist(diff);
        }
        min_dist = min(min_dist, dist);
    }
    min_dist += (1.0 - step(0.002, min_dist));
    return min_dist;
}

)"+1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

TEST_CASE("while")
{
    auto str = R"(

void test() {
    int i = 0;
    while (true) { }
    while (i < 10) { }
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
void test()
{
    int(i) = 0;
    while (true)
    {
    }
    while ((i < 10))
    {
    }
}

)"+1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

TEST_CASE("ternary")
{
    auto str = R"(

void main() {
    float a = 0, b = 0, c = 0, d = 0;
    float w = (a ? a,b : b,c);
    float z = a ? b ? c : d : w;
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
void main()
{
    float(a) = 0, b = 0, c = 0, d = 0;
    float(w) = (a ? a, b : b), c;
    float(z) = (a ? (b ? c : d) : w);
}

)"+1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

TEST_CASE("switch")
{
    auto str = R"(

void test() {
    // simple test
    int i = 10;
    switch (i) {
    case 0: break;
    default: break;
    }
    // nested
    switch (i) {
    case 0:
        switch (1) {
        case 1:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
void test()
{
    int(i) = 10;
    switch (i)
    {
        case 0:
            break;
        default:
            break;
    }
    switch (i)
    {
        case 0:
            switch (1)
            {
                case 1:
                    break;
                default:
                    break;
            }
        break;
        default:
            break;
    }
}

)" + 1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

//TEST_CASE("struct")
//{
//    auto str = R"(
//
//struct foo
//{
//    vec3 a;
//    vec2 b;
//    float c[100];
//};
//
//struct bar
//{
//    foo a, b, c;
//} a, b, c;
//
//void main( )
//{
//    bar a, b, c;
//}
//
//struct zz
//{
//    int i;
//}
//
//void main( )
//{
//    zz a;
//}
//
//)";
//
//    cslang::Parser parser(str);
//    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);
//
//    std::stringstream ss;
//    cslang::GenTranslationUnit(ss, stat);
//    std::cout << ss.str() << std::endl;
//
//    REQUIRE(ss.str() == R"(
//void main()
//{
//    float(a) = 0, b = 0, c = 0, d = 0;
//    float(w) = (a ? a, b : b), c;
//    float(z) = (a ? (b ? c : d) : w);
//}
//
//)" + 1);
//
//    cslang::GenTranslationUnit(std::cout, stat);
//}

TEST_CASE("for")
{
    auto str = R"(

void main() {
    int i = 0;
    for(;;) { }
    for(i = 0;; ) { }
    for(int i = 0; i < 10;) { }
    for(int i = 0; i < 10; i++) { }
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
void main()
{
    int(i) = 0;
    for (; ; )
    {
    }
    for (i = 0; ; )
    {
    }
    for (int(i) = 0; (i < 10); )
    {
    }
    for (int(i) = 0; (i < 10); i++)
    {
    }
}

)" + 1);

    //cslang::GenTranslationUnit(std::cout, stat);
}

TEST_CASE("do")
{
    auto str = R"(

void test() {
    do a(); while (true);
    do { } while (true);
}

)";

    cslang::Parser parser(str);
    auto stat = cslang::ast::DeclarationParser::ParseTranslationUnit(parser);

    std::stringstream ss;
    cslang::GenTranslationUnit(ss, stat);
    //std::cout << ss.str() << std::endl;

    REQUIRE(ss.str() == R"(
void test()
{
    do a(); while (true);
    do {
    } while (true);
}

)" + 1);

    //cslang::GenTranslationUnit(std::cout, stat);
}