#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include <QVector3D>

struct Matrix4x4 {
    double m[4][4];

    Matrix4x4() {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0 : 0.0;
    }

    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                result.m[i][j] = 0;
                for(int k = 0; k < 4; k++)
                    result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
        return result;
    }

    QVector3D transform(const QVector3D& v) const {
        double x = m[0][0]*v.x() + m[0][1]*v.y() + m[0][2]*v.z() + m[0][3];
        double y = m[1][0]*v.x() + m[1][1]*v.y() + m[1][2]*v.z() + m[1][3];
        double z = m[2][0]*v.x() + m[2][1]*v.y() + m[2][2]*v.z() + m[2][3];
        return QVector3D(x, y, z);
    }
};

#endif // MATRIX4X4_H
