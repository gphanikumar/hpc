class SOL_INIT
{
private:
  int CENTER ;
  int RADIUS ;
  float FOREGROUND ;
  float BACKGROUND ;
  
public:
  
  SOL_INIT(int centre, int radius, float foreground, float background){
    CENTER = centre;
    RADIUS = radius;
    FOREGROUND = foreground;
    BACKGROUND = background;
  }
  
  float init(int j, int i) {
    i -= CENTER;
    j -= CENTER;
    if (i*i + j*j <= RADIUS*RADIUS) return FOREGROUND;
    else return BACKGROUND;
  }

  float init_1(int j, int i) {
    i -= CENTER; i += 10;
    j -= CENTER;
    if (i*i + j*j <= 0.5) return FOREGROUND;
    else return BACKGROUND;
  }

  float init_2(int j, int i) {
    i -= CENTER; i -= 10;
    j -= CENTER;
    if (i*i + j*j <= 0.5) return FOREGROUND;
    else return BACKGROUND;
  }
};
