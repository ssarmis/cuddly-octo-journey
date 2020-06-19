#pragma once

struct v2 {
    r32 x;
    r32 y;

    v2():
    x(0), y(0){
    }

    v2(r32 data):
    x(data), y(data){
    }

    v2(r32 x, r32 y):
    x(x), y(y){
    }
};

struct v3 {
    r32 x;
    r32 y;
    r32 z;

    v3():
    x(0), y(0), z(0){
    }

    v3(r32 data):
    x(data), y(data), z(data){
    }

    v3(r32 x, r32 y, r32 z):
    x(x), y(y), z(z){
    }
};


struct v4 {
    r32 x;
    r32 y;
    r32 z;
    r32 w;

    v4():
    x(0), y(0), z(0), w(0){
    }

    v4(r32 data):
    x(data), y(data), z(data), w(data){
    }

    v4(r32 x, r32 y, r32 z, r32 w):
    x(x), y(y), z(z), w(w){
    }
};

struct m4 {
    r32 m[16];

    m4(){
        m[0]  = 1; m[1]  = 0; m[2]  = 0; m[3]  = 0;
        m[4]  = 0; m[5]  = 1; m[6]  = 0; m[7]  = 0; 
        m[8]  = 0; m[9]  = 0; m[10] = 1; m[11] = 0; 
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    }
};

static inline r32 dot(v3 a, v3 b){
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

// TODO(Sarmis) remove use of math.h
// is used for: tan, sin , cos
#include <math.h>

#define PI 3.14159

static inline m4 projection(r32 aspectRatio, r32 fov, r32 near, r32 far){
    m4 result = m4();

    fov *= 180.0 / PI;

    r32 cotFov = 1.0f / tan(fov / 2.0f);
    r32 frustumDepth = far - near;

    result.m[0] = cotFov / aspectRatio;
    result.m[5] = cotFov;

    result.m[10] = (far + near) / frustumDepth;
    result.m[11] = -(2.0f * far * near) / frustumDepth;

    result.m[14] = 1;
    result.m[15] = 0;

    return result;
}

static inline m4 scale(r32 size){
    m4 result = m4();
    
    result.m[0] = size;
    result.m[5] = size;
    result.m[10] = size;

    return result;
}

static inline m4 translate(v3 offset){
    m4 result = m4();

    result.m[3] = offset.x;
    result.m[7] = offset.y;
    result.m[11] = offset.z;

    return result;
}

static inline m4 rotateY(r32 angle){
    m4 result = m4();

    angle *= 180.0 / PI;

    result.m[0] = cos(angle);
    result.m[2] = -sin(angle);

    result.m[8] = sin(angle);
    result.m[10] = cos(angle);

    return result;
}

// TODO(Sarmis) implement this as SIMD
m4 operator*(m4 left, m4 right){
    m4 result = {};

    for(int row = 0; row < 4; ++row){
        for(int col = 0; col < 4; ++col){
            r32 sum = 0;
            for(int i = 0; i < 4; ++i){
                 sum += left.m[i + row * 4] * right.m[col + i * 4];
            }
            result.m[col + row * 4] = sum;
        }
    }

    return result;
}

void operator*=(m4& left, m4 right){
    left = left*right;
}

v3 operator*(v3 left, r32 right){
    v3 result = {};
    result.x = left.x * right;
    result.y = left.y * right;
    result.z = left.z * right;
    return result;
}

v3 operator*(r32 left, v3 right){
    return right * left;
}

v3 operator-(v3& vector){
    return v3(-vector.x, -vector.y, -vector.z);
}

v3 operator+(v3 left, v3 right){
    return v3(left.x + right.x, left.y + right.y, left.z + right.z);
}

v2 operator+(v2 left, v2 right){
    return v2(left.x + right.x, left.y + right.y);
}

v3 operator-(v3 left, v3 right){
    return v3(left.x - right.x, left.y - right.y, left.z - right.z);
}

void operator+=(v3& left, v3 right){
    left = left + right;
}

void operator+=(v2& left, v2 right){
    left = left + right;
}

void operator-=(v3& left, v3 right){
    left = left - right;
}

r32 length(v3 vector){
    r32 result = 0;
    result = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    return result;
}

v3 normalize(v3 vector){
    r32 magnitude = length(vector);
    return v3(vector.x / magnitude, vector.y / magnitude, vector.z / magnitude);
}

v3 cross(v3 left, v3 right){
    v3 result = v3();
    result.x = left.y * right.z - left.z * right.y;
    result.y = left.z * right.x - left.x * right.z;
    result.z = left.x * right.y - left.y * right.x;
    return result;
}

m4 lookAt(v3 up, v3 position, v3 target){
    m4 result = m4();

    v3 forward = normalize(target - position);
    v3 right = cross(forward, up);

    result.m[0] = right.x;
    result.m[1] = up.x;
    result.m[2] = forward.x;
    result.m[3] = position.x;

    result.m[4] = right.y;
    result.m[5] = up.y;
    result.m[6] = forward.y;
    result.m[7] = position.y;

    result.m[8] = right.z;
    result.m[9] = up.z;
    result.m[10] = forward.z;
    result.m[11] = position.z;

    return result;
}

m4 inverse(m4 matrix){
    m4 result = m4();

    result.m[0] =
             matrix.m[5]  * matrix.m[10] * matrix.m[15] - 
             matrix.m[5]  * matrix.m[11] * matrix.m[14] - 
             matrix.m[9]  * matrix.m[6]  * matrix.m[15] + 
             matrix.m[9]  * matrix.m[7]  * matrix.m[14] +
             matrix.m[13] * matrix.m[6]  * matrix.m[11] - 
             matrix.m[13] * matrix.m[7]  * matrix.m[10];

    result.m[4] =
             -matrix.m[4]  * matrix.m[10] * matrix.m[15] + 
              matrix.m[4]  * matrix.m[11] * matrix.m[14] + 
              matrix.m[8]  * matrix.m[6]  * matrix.m[15] - 
              matrix.m[8]  * matrix.m[7]  * matrix.m[14] - 
              matrix.m[12] * matrix.m[6]  * matrix.m[11] + 
              matrix.m[12] * matrix.m[7]  * matrix.m[10];

    result.m[8] =
             matrix.m[4]  * matrix.m[9] * matrix.m[15] - 
             matrix.m[4]  * matrix.m[11] * matrix.m[13] - 
             matrix.m[8]  * matrix.m[5] * matrix.m[15] + 
             matrix.m[8]  * matrix.m[7] * matrix.m[13] + 
             matrix.m[12] * matrix.m[5] * matrix.m[11] - 
             matrix.m[12] * matrix.m[7] * matrix.m[9];

    result.m[12] =
              -matrix.m[4]  * matrix.m[9] * matrix.m[14] + 
               matrix.m[4]  * matrix.m[10] * matrix.m[13] +
               matrix.m[8]  * matrix.m[5] * matrix.m[14] - 
               matrix.m[8]  * matrix.m[6] * matrix.m[13] - 
               matrix.m[12] * matrix.m[5] * matrix.m[10] + 
               matrix.m[12] * matrix.m[6] * matrix.m[9];

    result.m[1] =
             -matrix.m[1]  * matrix.m[10] * matrix.m[15] + 
              matrix.m[1]  * matrix.m[11] * matrix.m[14] + 
              matrix.m[9]  * matrix.m[2] * matrix.m[15] - 
              matrix.m[9]  * matrix.m[3] * matrix.m[14] - 
              matrix.m[13] * matrix.m[2] * matrix.m[11] + 
              matrix.m[13] * matrix.m[3] * matrix.m[10];

    result.m[5] =
             matrix.m[0]  * matrix.m[10] * matrix.m[15] - 
             matrix.m[0]  * matrix.m[11] * matrix.m[14] - 
             matrix.m[8]  * matrix.m[2] * matrix.m[15] + 
             matrix.m[8]  * matrix.m[3] * matrix.m[14] + 
             matrix.m[12] * matrix.m[2] * matrix.m[11] - 
             matrix.m[12] * matrix.m[3] * matrix.m[10];

    result.m[9] =
             -matrix.m[0]  * matrix.m[9] * matrix.m[15] + 
              matrix.m[0]  * matrix.m[11] * matrix.m[13] + 
              matrix.m[8]  * matrix.m[1] * matrix.m[15] - 
              matrix.m[8]  * matrix.m[3] * matrix.m[13] - 
              matrix.m[12] * matrix.m[1] * matrix.m[11] + 
              matrix.m[12] * matrix.m[3] * matrix.m[9];

    result.m[13] =
              matrix.m[0]  * matrix.m[9] * matrix.m[14] - 
              matrix.m[0]  * matrix.m[10] * matrix.m[13] - 
              matrix.m[8]  * matrix.m[1] * matrix.m[14] + 
              matrix.m[8]  * matrix.m[2] * matrix.m[13] + 
              matrix.m[12] * matrix.m[1] * matrix.m[10] - 
              matrix.m[12] * matrix.m[2] * matrix.m[9];

    result.m[2] =
             matrix.m[1]  * matrix.m[6] * matrix.m[15] - 
             matrix.m[1]  * matrix.m[7] * matrix.m[14] - 
             matrix.m[5]  * matrix.m[2] * matrix.m[15] + 
             matrix.m[5]  * matrix.m[3] * matrix.m[14] + 
             matrix.m[13] * matrix.m[2] * matrix.m[7] - 
             matrix.m[13] * matrix.m[3] * matrix.m[6];

    result.m[6] =
             -matrix.m[0]  * matrix.m[6] * matrix.m[15] + 
              matrix.m[0]  * matrix.m[7] * matrix.m[14] + 
              matrix.m[4]  * matrix.m[2] * matrix.m[15] - 
              matrix.m[4]  * matrix.m[3] * matrix.m[14] - 
              matrix.m[12] * matrix.m[2] * matrix.m[7] + 
              matrix.m[12] * matrix.m[3] * matrix.m[6];

    result.m[10] =
              matrix.m[0]  * matrix.m[5] * matrix.m[15] - 
              matrix.m[0]  * matrix.m[7] * matrix.m[13] - 
              matrix.m[4]  * matrix.m[1] * matrix.m[15] + 
              matrix.m[4]  * matrix.m[3] * matrix.m[13] + 
              matrix.m[12] * matrix.m[1] * matrix.m[7] - 
              matrix.m[12] * matrix.m[3] * matrix.m[5];

    result.m[14] =
              -matrix.m[0]  * matrix.m[5] * matrix.m[14] + 
               matrix.m[0]  * matrix.m[6] * matrix.m[13] + 
               matrix.m[4]  * matrix.m[1] * matrix.m[14] - 
               matrix.m[4]  * matrix.m[2] * matrix.m[13] - 
               matrix.m[12] * matrix.m[1] * matrix.m[6] + 
               matrix.m[12] * matrix.m[2] * matrix.m[5];

    result.m[3] =
             -matrix.m[1] * matrix.m[6] * matrix.m[11] + 
              matrix.m[1] * matrix.m[7] * matrix.m[10] + 
              matrix.m[5] * matrix.m[2] * matrix.m[11] - 
              matrix.m[5] * matrix.m[3] * matrix.m[10] - 
              matrix.m[9] * matrix.m[2] * matrix.m[7] + 
              matrix.m[9] * matrix.m[3] * matrix.m[6];

    result.m[7] =
             matrix.m[0] * matrix.m[6] * matrix.m[11] - 
             matrix.m[0] * matrix.m[7] * matrix.m[10] - 
             matrix.m[4] * matrix.m[2] * matrix.m[11] + 
             matrix.m[4] * matrix.m[3] * matrix.m[10] + 
             matrix.m[8] * matrix.m[2] * matrix.m[7] - 
             matrix.m[8] * matrix.m[3] * matrix.m[6];

    result.m[11] =
              -matrix.m[0] * matrix.m[5] * matrix.m[11] + 
               matrix.m[0] * matrix.m[7] * matrix.m[9] + 
               matrix.m[4] * matrix.m[1] * matrix.m[11] - 
               matrix.m[4] * matrix.m[3] * matrix.m[9] - 
               matrix.m[8] * matrix.m[1] * matrix.m[7] + 
               matrix.m[8] * matrix.m[3] * matrix.m[5];

    result.m[15] =
              matrix.m[0] * matrix.m[5] * matrix.m[10] - 
              matrix.m[0] * matrix.m[6] * matrix.m[9] - 
              matrix.m[4] * matrix.m[1] * matrix.m[10] + 
              matrix.m[4] * matrix.m[2] * matrix.m[9] + 
              matrix.m[8] * matrix.m[1] * matrix.m[6] - 
              matrix.m[8] * matrix.m[2] * matrix.m[5];

    r32 determinant =
          matrix.m[0] * result.m[0] + 
          matrix.m[1] * result.m[4] + 
          matrix.m[2] * result.m[8] +
          matrix.m[3] * result.m[12];

    // TODO(Sarmis) handle this
    if (determinant == 0){
        return result;
    }

    determinant = 1.0 / determinant;

    for (int i = 0; i < 16; i++){
        result.m[i] *= determinant;
    }

    return result;
}

#if 0


a 0 0 0    x     x * a
0 b 0 0    y     y * b
0 0 1 0    z       z
0 0 0 1    w       w

left = 0
top = 0
bottom = 780
right = 1200

left * a = -1 => a = -(1 / left)
right * a = 1 => a = 1 / right

                 2a = (1 / right) - (1 / left) 
                  a = ((1 / right) - (1 / left)) / 2

top * b = 1     => b = 1 / top
bottom * b = -1 => b = -(1 / bottom)
                   
                   2b = (1 / top) - (1 / bottom)
                   b = ((1 / top) - (1 / bottom)) / 2


#endif





m4 orthographic(r32 left, r32 right, r32 top, r32 bottom){
    m4 result = m4();

    result.m[0] = 2.0 / (right - left);
    result.m[3] = -(right + left) / (right - left);

    result.m[5] = 2.0 / (top - bottom);
    result.m[7] = -(top + bottom) / (top - bottom);

    return result;
}

i32 clamp(i32 value, i32 min, i32 max){
    if(value < min){
        return min;
    } else if (value > max) {
        return max;
    }
    return value;
}

#if 0
    result.m[0] = right.x;
    result.m[1] = right.y;
    result.m[2] = right.z;
    result.m[3] = 0;

    result.m[4] = up.x;
    result.m[5] = up.y;
    result.m[6] = up.z;
    result.m[7] = 0;

    result.m[8] = forward.x;
    result.m[9] = forward.y;
    result.m[10] = forward.z;
    result.m[11] = 0;

    result.m[12] = position.x;
    result.m[13] = position.y;
    result.m[14] = position.z;
    result.m[15] = 0;

#endif

v3 lerp(v3 start, v3 end, r32 time){
    return (start + (end - start) * time);
}



