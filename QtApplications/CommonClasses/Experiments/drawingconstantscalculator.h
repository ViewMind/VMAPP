#ifndef DRAWINGCONSTANTSCALCULATOR_H
#define DRAWINGCONSTANTSCALCULATOR_H

#include <QtMath>

namespace ReferenceResolution {
    static const qreal width  = 1920;
    static const qreal height = 1080;
};

/**
 * @brief The DrawingConstantsCalculator class
 * @details The purpose of this class is to compute the pixel value of the vertical and horizontal distances for display in a given resolution.
 * Basically all constant values for rendering squares, circles, line, etc were defined as a ratio of either the width or height of a reference resolution
 * Let Wr be the reference width.
 * Let Hr be the reference height.
 * The ratio for horizontal spaces is named a and the resulting reference value would be a*Wr
 * The ratio for vertical spaces is named b and the resulting reference value would be b*Hr.
 *
 * So given now a completely different resolution WxH we need to compute new horizontal and vertical values (x and y, respectively)
 * such that the aspect ration and area proportion of a square in the reference space matches aspect ratio and area proportion
 * in the target space.
 *
 * Same aspect ratio means: a.Wr/b.Hr = x.W/y.H.
 * Same area ratio means a.b.Wr.Hr/Wr*Hr = x.y.W.H/W.H which is equivalent to a.b = x.y
 *
 * Let K be Hr.W/Wr.H then the math says that:
 * x = a/sqrt(K)
 * y = b*sqrt(K);
 */

class DrawingConstantsCalculator
{
public:
    DrawingConstantsCalculator();

    /**
     * @brief setTargetResolution
     * @param w The width resolution of the screen to be drawn
     * @param h The hegiht resolution of the screen to be drawn
     */
    void setTargetResolution(qreal w, qreal h);
    qreal getVerticalRatio(qreal ref_ratio);
    qreal getHorizontalRatio(qreal ref_ratio);

private:
    qreal targetW;
    qreal targetH;
    qreal K;
    qreal sqrtK;

    void computeK();

};

#endif // DRAWINGCONSTANTSCALCULATOR_H
