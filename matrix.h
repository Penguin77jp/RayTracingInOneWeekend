#pragma once

/*
m11 m12 m13
m21 m22 m23
m31 m32 m33
*/
class mat3x3 {
public:
  float m11, m12, m13, m21, m22, m23, m31, m32, m33;
  mat3x3 (float m11, float m12, float m13, float m21, float m22, float m23,float m31, float m32, float m33) : m11(m11),m12(m12),m13(m13),m21(m21),m22(m22),m23(m23),m31(m31),m32(m32),m33(m33){}

  float det() {
    return m11 * m22 * m33 + m12 * m23 * m31 + m13 * m21 * m32 - m13 * m22 * m31 - m12 * m21 * m33 - m11 * m23 * m32;
  }
};
