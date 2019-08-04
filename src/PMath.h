namespace pas {
    namespace math {
        const float kPi = 3.141592654f;
        const float k2Pi = 6.283185307f;

        float RadiansToDegrees(float radians)
        {
            return (180.f/(float)kPi) * radians;
        }

        float DegreesToRadians(float degrees)
        {
            return ((float)kPi/180.f) * degrees;
        }
    }
}