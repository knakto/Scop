#include <RotationMatrix.hpp>

Matrix4x4::Matrix4x4(void)
: _moveScale(0.01f), _rotateScale(1), _zoomScale(0.01f)
{
  _matrix[0] = 1; _matrix[4] = 0; _matrix[8] = 0; _matrix[12] = 0;
  _matrix[1] = 0; _matrix[5] = 1; _matrix[9] = 0; _matrix[13] = 0;
  _matrix[2] = 0; _matrix[6] = 0; _matrix[10] = 1; _matrix[14] = 0;
  _matrix[3] = 0; _matrix[7] = 0; _matrix[11] = 0; _matrix[15] = 1;
}

Matrix4x4::Matrix4x4(float moveScale, float rotateScale, float zoomScale)
: _moveScale(moveScale * 0.01f), _rotateScale(rotateScale), _zoomScale(zoomScale)
{
  _matrix[0] = 1; _matrix[4] = 0; _matrix[8] = 0; _matrix[12] = 0;
  _matrix[1] = 0; _matrix[5] = 1; _matrix[9] = 0; _matrix[13] = 0;
  _matrix[2] = 0; _matrix[6] = 0; _matrix[10] = 1; _matrix[14] = 0;
  _matrix[3] = 0; _matrix[7] = 0; _matrix[11] = 0; _matrix[15] = 1;
}

Matrix4x4::~Matrix4x4(void){}

void Matrix4x4::clearMatrix(void)
{
  _matrix[0] = 1; _matrix[4] = 0; _matrix[8] = 0; _matrix[12] = 0;
  _matrix[1] = 0; _matrix[5] = 1; _matrix[9] = 0; _matrix[13] = 0;
  _matrix[2] = 0; _matrix[6] = 0; _matrix[10] = 1; _matrix[14] = 0;
  _matrix[3] = 0; _matrix[7] = 0; _matrix[11] = 0; _matrix[15] = 1;
}

void Matrix4x4::rotateX(e_mode mode)
{
  if (mode == Matrix4x4::UP)
    _angleX += _rotateScale;
  else if (mode == Matrix4x4::DOWN)
    _angleX -= _rotateScale;
  if (_angleX > 360)
    _angleX = 0;
  if (_angleX < 0)
    _angleX = 360;
}

void Matrix4x4::rotateY(e_mode mode)
{
  if (mode == Matrix4x4::UP)
    _angleY += _rotateScale;
  else if (mode == Matrix4x4::DOWN)
    _angleY -= _rotateScale;
  if (_angleY > 360)
    _angleY = 0;
  if (_angleY < 0)
    _angleY = 360;
}

void Matrix4x4::rotateZ(e_mode mode)
{
  if (mode == Matrix4x4::UP)
    _angleZ += _rotateScale;
  else if (mode == Matrix4x4::DOWN)
    _angleZ -= _rotateScale;
  if (_angleZ > 360)
    _angleZ = 0;
  if (_angleZ < 0)
    _angleZ = 360;
}

void Matrix4x4::translateX(e_mode mode)
{
  if (mode == Matrix4x4::UP)
    _moveX += _moveScale;
  else if (mode == Matrix4x4::DOWN)
    _moveX -= _moveScale;
}

void Matrix4x4::translateY(e_mode mode)
{
  if (mode == Matrix4x4::UP)
    _moveY += _moveScale;
  else if (mode == Matrix4x4::DOWN)
    _moveY -= _moveScale;
}

void Matrix4x4::translateZ(e_mode mode)
{
  if (mode == Matrix4x4::UP)
    _moveZ += _moveScale;
  else if (mode == Matrix4x4::DOWN)
    _moveZ -= _moveScale;
}

void Matrix4x4::zoomMatrix(e_mode mode)
{
  if (mode == Matrix4x4::UP)
    _zoom += _zoomScale;
  else if (mode == Matrix4x4::DOWN)
    _zoom -= _zoomScale;
  if (_zoom < 0.1)
    _zoom = 0.1;
  if (_zoom > 5.5)
    _zoom = 5.5;
}

static void multiplyMatrix(float resMatrix[16], float first[16], float second[16])
{
  float tmpMatrix[16]{};

  for (int row = 0; row < 4; row++)
  {
    for (int col = 0; col < 4; col++)
    {
      tmpMatrix[row + col * 4]
        = (first[row + 0] * second[0 + (col * 4)])
        + (first[row + 4] * second[1 + (col * 4)])
        + (first[row + 8] * second[2 + (col * 4)])
        + (first[row + 12] * second[3 + (col * 4)]);
    }
  }
  for (int i = 0; i < 16; i++)
    resMatrix[i] = tmpMatrix[i];
}

static void getRotationMatrix(float matrix[16], float angleX, float angleY, float angleZ)
{
  //rotate X axis
  {
    float tmp[16]{
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    };
    double rad = angleX * (M_PI / 180.0f);
    double c = cos(rad);
    double s = sin(rad);

    tmp[5] = c;
    tmp[10] = c;
    tmp[9] = -s;
    tmp[6] = s;
    multiplyMatrix(matrix, matrix, tmp);
  }

  //rotate Y axis
  {
    float tmp[16]{
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    };
    double rad = angleY * (M_PI / 180.0f);
    double c = cos(rad);
    double s = sin(rad);

    tmp[0] = c;
    tmp[10] = c;
    tmp[2] = -s;
    tmp[8] = s;
    multiplyMatrix(matrix, matrix, tmp);
  }

  //rotate Z axis
  {
    float tmp[16]{
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    };
    double rad = angleZ * (M_PI / 180.0f);
    double c = cos(rad);
    double s = sin(rad);

    tmp[0] = c;
    tmp[5] = c;
    tmp[4] = -s;
    tmp[1] = s;
    multiplyMatrix(matrix, matrix, tmp);
  }
}

static void createPerspective(float fovDegrees, float aspectRatio, float zNear, float zFar, float matrix[16])
{
    float result[16]{};
    
    // 1. แปลงองศาให้เป็นเรเดียน (คอมพิวเตอร์กินเรเดียนเป็นอาหาร)
    float fovRadians = fovDegrees * (M_PI / 180.0f);
    
    // 2. คำนวณค่าตัวคูณของเลนส์ (focal length)
    // ใช้สูตร 1.0 / tan(มุม / 2)
    float tanHalfFov = std::tan(fovRadians / 2.0f);
    float f = 1.0f / tanHalfFov;
    
    // 3. หยอดตัวเลขลงในช่องเมทริกซ์ 4x4 (แบบ Column-Major)
    // คอลัมน์ที่ 0 (แกน X: บีบเข้าตามสัดส่วนหน้าจอ)
    result[0]  = f / aspectRatio; 
    
    // คอลัมน์ที่ 1 (แกน Y: บีบเข้าตามระยะเลนส์)
    result[5]  = f;               
    
    // คอลัมน์ที่ 2 (แกน Z: คำนวณการเรียงลำดับความลึก Near/Far)
    result[10] = -(zFar + zNear) / (zFar - zNear);
    result[11] = -1.0f; // หัวใจสำคัญ: ส่งค่า Z เก่าไปหารให้ของที่อยู่ไกลเล็กลง (Perspective Divide)
    
    // คอลัมน์ที่ 3 (การเลื่อนแกน Z)
    result[14] = -(2.0f * zFar * zNear) / (zFar - zNear);

    multiplyMatrix(matrix, result, matrix);
}

void Matrix4x4::calculateMatrix(void)
{
  float rotateMatrix[16]{
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
  };
  getRotationMatrix(rotateMatrix, _angleX, _angleY, _angleZ);
  rotateMatrix[12] = _moveX;
  rotateMatrix[13] = _moveY;
  rotateMatrix[14] = _moveZ;

  rotateMatrix[0] *= _zoom;
  rotateMatrix[1] *= _zoom;
  rotateMatrix[2] *= _zoom;
  rotateMatrix[4] *= _zoom;
  rotateMatrix[5] *= _zoom;
  rotateMatrix[6] *= _zoom;
  rotateMatrix[8] *= _zoom;
  rotateMatrix[9] *= _zoom;
  rotateMatrix[10] *= _zoom;
  createPerspective(60.0f, 1920.0f / 1080.0f, 0.1f, 100.0f, rotateMatrix);
  for (int i = 0; i < 16; i++)
    _matrix[i] = rotateMatrix[i];
}
