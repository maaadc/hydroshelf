
#ifndef HYDROSHELF_MATRIX_DISPLAY_HPP
#define HYDROSHELF_MATRIX_DISPLAY_HPP

namespace hydroshelf
{
using rgb_t = std::array<uint8_t, 3>;

constexpr rgb_t ColorRed       = {0x40, 0x00, 0x00};
constexpr rgb_t ColorDarkRed   = {0x20, 0x00, 0x00};
constexpr rgb_t ColorGreen     = {0x00, 0x40, 0x00};
constexpr rgb_t ColorDarkGreen = {0x00, 0x20, 0x00};
constexpr rgb_t ColorBlue      = {0x00, 0x00, 0x40};
constexpr rgb_t ColorDarkBlue  = {0x00, 0x00, 0x20};
constexpr rgb_t ColorYellow    = {0x40, 0x30, 0x00};
constexpr rgb_t ColorWhite     = {0x40, 0x40, 0x40};
constexpr rgb_t ColorGray      = {0x20, 0x20, 0x20};
constexpr rgb_t ColorBlack     = {0x00, 0x00, 0x00};

template <int Width, int Height> class TMatrixDisplay
{
  public:
    TMatrixDisplay()
        : m_bufferRgb()
    {
        m_bufferRgb[0] = 0x05;
        m_bufferRgb[1] = 0x05;
    }

    ~TMatrixDisplay() = default;

    inline int getHeight()
    {
        return Height;
    }

    inline int getWidth()
    {
        return Width;
    }

    inline void showBinaryClock(int32_t timeSec)
    {
        assert(timeSec >= 0 && timeSec < 86400);

        uint32_t cntHours   = timeSec / 3600;
        uint32_t cntMinutes = (timeSec % 3600) / 60;
        uint32_t cntSeconds = timeSec % (3600 * 60);

        fillBlack();

        for (int k = 0; k < 3; k++)
        {
            if (cntHours & (1U << k))
                setColor(ColorBlue, 0, k);
            if (cntHours & (1U << k << 3))
                setColor(ColorBlue, 1, k);
            if (cntMinutes & (1U << k))
                setColor(ColorGreen, 2, k);
            if (cntMinutes & (1U << k << 3))
                setColor(ColorGreen, 3, k);
        }
        setColor(ColorRed, 4, cntSeconds % 3);
    }

    inline void showProgressBar(const rgb_t &f_color, const float f_value, const int f_y)
    {
        assert(f_value >= 0.F);
        assert(f_value <= 1.F);

        const int pixelValue = Width * f_value;
        for (int x = 0; x < Width; ++x)
        {
            setColor((pixelValue > x) ? f_color : ColorBlack, x, f_y);
        }
    }

    inline void drawProgressBarTwoLines(const rgb_t &f_color, const float f_value, const int f_y)
    {
        showProgressBar(f_color, clip(0.F, 2.F * f_value, 1.F), f_y);
        showProgressBar(f_color, clip(0.F, 2.F * f_value - 1.F, 1.F), f_y + 1);
    }

    inline void showWaitBar(const rgb_t &f_color, const int f_value, const int f_y)
    {
        const int lightPos = f_value % Width;
        for (int x = 0; x < Width; ++x)
        {
            setColor((lightPos == x) ? f_color : ColorBlack, x, f_y);
        }
    }

    inline void show()
    {
        M5.dis.displaybuff(m_bufferRgb.data());
    }

    inline void setColor(const rgb_t &f_color, const int f_x, const int f_y)
    {
        assert(f_x >= 0 && f_x < Width);
        assert(f_y >= 0 && f_y < Height);

        int32_t idx = f_y * Width + f_x;
        setColor(f_color, idx);
    }

    inline void setColor(const rgb_t &f_color, const int32_t f_idx)
    {
        assert(f_idx >= 0 && f_idx < Width * Height);
        assert(2 + 3 * (f_idx + 1) <= m_bufferRgb.size());

        for (int32_t c = 0; c < 3; c++)
        {
            m_bufferRgb[2 + 3 * f_idx + c] = f_color[c];
        }
    }

    inline void fillColor(const rgb_t &f_color)
    {
        for (int32_t idx = 0; idx < Width * Height; ++idx)
        {
            setColor(f_color, idx);
        }
    }

    inline void fillBlack()
    {
        for (int32_t idx = 2; idx < m_bufferRgb.size(); ++idx)
        {
            m_bufferRgb[idx] = 0x00;
        }
    }

  private:
    std::array<uint8_t, 2 + Width *Height * 3> m_bufferRgb = {0U};
};

using CMatrixDisplay = TMatrixDisplay<5, 5>;

} // namespace hydroshelf

#endif // HYDROSHELF_MATRIX_DISPLAY_HPP