A - 1. 구현한 내용
1. 도형이 좌측 혹은 우측에서 날아온다. 도형의 종류가 여러가지이다.
	저장 형태 : polygons에 정점들이 indexTriangles에 인덱스 삼각형 리스트의 각 도형의 원본들이 저장되어 있고 onShape와 onShapeindex에는 현재 화면에 그려지는 도형들이 저장된다.
	구현 방법 : TimerFunction(0)에 의해 일정 시간(3초)마다 throw_new_shape()가 호출되고 polygons에 저장되어 있는 도형중 랜덤하게 하나를 onShape에 정점들 onShapeindex에 인덱스삼각형리스트를 복사하여 저장한다.
			이후 location에 시작할 위치 glm::vec3(-1 or 1, 랜덤y좌표, 0), velocity에 속도 glm::vec3(반대편으로 날아가게 랜덤 x속도, 위쪽으로 날아가게 랜덤 y속도,0), colors에는 color_ref의 랜덤한 색상 중 하나를 저장한다.
			이때 onShape, onShapeindex, location, velocity, colors의 동일한 인덱스에는 동일한 도형에 관한 정보들이 저장된다.
			TimerFunction(1)에서는 velocity와 location을 업데이트 하는데 velocity에는 중력을 계속해서 더해줘 점점 밑으로 떨어지게 하였고 location에는 그런 velocity를 더하며 위치를 이동시켰다.
	
2. 바구니가 좌우로 움직인다. 떨어진 조각이 바구니에 닿으면 바구니에 담아진다.
	저장 형태 : 바구니는 Bucket이라는 class로 구현되어 있고 그 안에는 바구니와 바구니 안의 도형들에 대한 VAO,VBO,EBO / 정점버퍼와 인덱스버퍼 / location, velocity / 바구니의 size와 도형의 color들과 바구니에 관한 함수들이 있다.
	구현 방법 : 도형이 떨어지다 y가 -0.6이하가 되면 바구에 들어갔는지 bucket.is_in_bucket(location, index)으로 체크한다. 함수 내에서는 해당 location이 바구니의 안쪽에 있는지 확인한 다음에 안쪽에 있다면 push_in_bucket(index)를 실행한다. push_in_bucket(index)에서는 해당 인덱스의 onShape, onShapeindex, location, velocity, colors를 복사하여 클래스 안의 벡터에 저장한다. 이때 velocity.x는 0으로 만들어 좌우로 이동하지 않게 한다. 이후 destroyFunc(index)를 호출하여 해당 인덱스의 도형을 삭제한다. bucket.is_in_bucket(location, index)에서 false라 리턴되면 바구니 빡에 있는 도형이고 이때에는 bucket.toched_boundary(location, i)를 검사하여 바구니에 닿은 도형이면 bucket.add_velocity(index)를 통해 해당 도형에 bucket의 속도 * 1.1을 더하여 날려보낸다.
			바구니와 바구니 안의 도형들의 관한 이동은 도형의 이동과 거의 같다.velocity를 업데이트하고 location에 더하여 위치를 이동시키고 location이 -1과 1을 벗어나려 하면(화면 밖으로 나가려고 하면) velocity.x를 반전시켜 반대방향으로 이동시킨다. 바구니 안의 도형은 기본적으로 바구니와 같이 움직이는데 만약 공중에 떠 있다면 gravity를 velocity에 더하여 밑으로 떨어지게 하였다. 각각의 도형들은 Set_inBucket(index)에서 VAO와 VBO, EBO를 초기화 하고 Draw_inBucket()에서 그려진다.

3. 절취선 그리기
	저장형태 : 절취선은 Mouse_Line이라는 클래스의 형태로 구현하였고 시작점과 끝점을 각각 start_x, start_y, end_x, end_y로 저장하였다. start()와 end()를 통해 private인 해당 값들을 glm::vec2의 형태로 읽을 수 있다. 클릭한 상태인지는 b_ML1이라는 bool로 저장하였다. 
	구현 방법 : 마우스 클릭시 ML1.set_start(mx, my), ML1.set_end(mx, my) 를 호출하여 start_x, start_y, end_x, end_y를 설정하고 b_ML1을 true로 바꾼다. MouseMotion에서 b_ML이 true면 ML1.set_end(mx, my)를 지속적으로 호출하여 끝점을 계속해서 갱신한다. 이때 mx, my는 마우스 입력의 x, y값을 -1 ~ 1사이의 값으로 변환한 값이다. 마우스 클릭을 해제하면 마지막으로 ML1.set_end(mx, my)를 호출하여 끝점을 결정하고 모든 도형에 대하여 checkIntersections(ML1.start(), ML1.end(), onShape[i], i)를 실행한다. Draw_Line에서 해당 좌표값들을 std::vector<glm::vec3>의 형태로 변환하여 화면에 출력한다.

4. 절취연산
	기본 개념 : checkIntersections에서 도형의 각 정점에 대한 화면에서의 좌표를 계산하여 doIntersect로 넘긴다. doIntersect에서는 마우스 선분과 도형의 두 정점을 잇는 선분이 실제로 만나는지 계산하고 만난다면 만나는 지점을 intersection에 저장하고 true를 반환한다. true이면 반환된 intersection값을 다시 -1~1인 도형 좌표계로 변환하여 intersectionPoints에 저장한다. 만나는 지점이 2개 미만이면 intersectionPoints를 초기화 하고 false를 리턴한다. 2개 이상이면 true를 리턴한다. true가 리턴되었다면 SplitPolygonByIntersection(i)를 호출한다. SplitPolygonByIntersection에서는 onShape에서 기존 도형의 정점들을 불러 intersectionPoints의 정점을 기준으로 분리하여 2개의 도형으로 나눈다. 이에 따라 
onShapeindex, location, velocity, colors와 후술할 경로저장 벡터인 paths도 2개를 따로 저장한다. 이후 intersectionPoints를 초기화 한다.
	doIntersect에서 연산 하는 방법 : 마우스 선분과 도형의 두 정점을 잇는 선분을 나타내는 벡터 r과 s를 초기화 하고 두 벡터를 외적하여 평행하는지 확인한다(rxs == 0). 이후 평행하지 않다면 교차 지점의 비율인 t와 u를 계산하여 둘 모두 0과 1사이의 값이면 교차 지점이 두 선분의 범위 내에 존재한다는 뜻이다. 이때 t = (q1-p1) 과 s의 외적 / rxs, u = (q1 - p1)과 r의 외적 / rxs이다. 둘 모두 0과 1 사이의 값이라면 두 선분이 교차한다는 뜻이고 교차점은 p1 + t * r (p1과 p2에 관한 t의 매게변수 방정식)으로 구할 수 있다.
	checkIntersections에서 각 도형의 정점의 화면상 좌표를 구하고 다시 도형좌표계로 변환한 방법 : 각 도형의 정점에 도형의 변환행렬을 적용시킨 것처럼 location을 더하고 사이즈를 0.1을 곱한다. 이후 교차점을 구하면 교차점에 location을 빼주어 원점으로 되돌리고 10을 곱해 다시 -1~1의 좌표계인 도형좌표계로 변환시킨다.
	SplitPolygonByIntersection에서 도형을 나누는 방법 : originalVertices에 기존 도형의 정점들을 복사하고 새 도형의 정점버퍼와 인덱스 버퍼를 저장할 newVertices1,2 / newIndices1,2를 만든다. p1,p2에 두 교차점을 초기화 시키고 p2-p1과 도형의각정점-p1의 외적을 계산해 p2-p1을 기준으로 왼쪽이나 위에 있는 정점들과 오른쪽이나 아래에 있는 정점으로 나누어 저장한다. 이후 기준점이 된 p1과 p2도 저장한 다음에 CreateTriangleIndexList(newVertices) 함수를 통해 각 정점에서 모든 정점과의 삼각형을 만들어 newIndices1에 삼각형 리스트를 저장한다. 이후 기존 도형의 정보들을 복사하여 각 도형에 저장한 다음 왼쪽에 있던 정점들로 만들어진 도형은 살짝 왼쪽 위로 속력을 추가하고 오른쪽에 있던 정점들로 만들어진 도형은 오른쪽 아래로 속력을 추가하여 다른 방향으로 떨어지게 만들었다.

5. 화면 밖으로 나가면 사라진다.
	구현 방법 : 도형의 location.y가 -1 이하가 되면 destroyFunc(index)가 호출되어 도형이 사라진다.

6. 도형의 모드(LINE/FILL)
	구현 방법 : isFill이라는 bool값을 설정하여 false면 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); true면 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)로 설정하여 도형이 선이거나 면으로 출력된다.

7. 경로 출력하기
	저장 형태 : Path라는 클래스를 만들어 각 도형마다의 경로를 저장하게 만들었고 Path의 벡터를 만들어 여러개의 도형의 경로들을 저장하였다. 
	구현 방법 : TimerFunction(2)에서 300ms마다 paths[i]에 현재 i 도형의 위치를 저장한다. 이때 Path의 함수 setpoint를 호출하여 path내의 벡터에 저장하게 된다. 출력은 Path의 Draw_Line을 통해 벡터내의 모든 정점을 잇는 선분을 그린다. 해당 경로로 지나간 도형이 분리된다면 분리된 시점부터 따로 경로가 저장된다(이전에 날아온 경로는 유지된다.). 해당 경로로 지나간 도형이 -1이하로 내려가거나 바구니에 담겨 destroyFunc가 호출된다면 해당 경로또한 삭제된다.

8. 날아오는 속도 조절

A - 2. 보너스 요소
1. 여러번 슬라이스, 자유 슬라이스
2. 도형이 회전하며 날아오고 회전한 형태에서 슬라이스 가능
3. 날아오는 경로의 다양화(랜덤한 경로)

B. 구현하지 못한 내용
1. 도형이 '지나갈' 경로를 미리 출력하지는 못한다.


C. 키보드/마우스 명령어
1. 마우스 클릭 - 드래그 - 클릭해제 : 클릭 해제시 선에 닿은 도형 슬라이스


