#include <iostream>
#include <algorithm>
#include "pathFinder.h"


/*
   ###################################################################################
   ##############################  Position Ŭ���� ����  ##############################
   ###################################################################################
*/
Position::Position()
	: x(0), y(0)
{

}

Position::Position(int _x, int _y)
	: x(_x), y(_y)
{

}
bool Position::operator<(const Position& pos) const {
	if (x == pos.x)
		return (y < pos.y);
	else
		return (x < pos.x);
}

/*
   ###################################################################################
   #############################  pathFinder Ŭ���� ����  #############################
   ###################################################################################
*/
void PathFinder::setMoveInterval(int _move) {
	move = _move;
}

/* �־��� ���� ���� Ž���ϴ� ��ǥ List ��ȯ */
std::list<Position> PathFinder::findCoveragePath(Position start, cv::Mat mapImage, int show){
	now = start;	// test 80, 80
	map.setMap(mapImage.cols, mapImage.rows, mapImage.data);
	cv::Mat resultOrigin(map.getMapHeight(), map.getMapWidth(), CV_8UC1, map.getMapAddr());

	if (show) {
		cv::namedWindow("resultOrigin", cv::WINDOW_NORMAL);
		cv::imshow("origin", mapImage);
	}
	
	for (int j = start.y % move; j < map.getMapHeight(); j += move) {
		for (int i = start.x % move; i < map.getMapWidth(); i += move) {
			if (map.getMapData(i, j) == LOAD) {
				backTrackingList.insert(Position(i, j));
			}
		}
	}

	Astar astar;
	myNode* fin;
	astar.setMap(map);

	int ret;
	Position Nearest;
	std::set<Position>::iterator findPos;
	std::list<Position> pathList;

	printf("\n���Ž�� ����\n\n");
	pathList.push_back(now);

	while (!backTrackingList.empty()) {
		if (show) {
			cv::imshow("resultOrigin", resultOrigin);
			cv::waitKey(1);
		}
		
		map.setMapData(now.x, now.y, CLEANED);

		findPos = backTrackingList.find(now);
		if (findPos != backTrackingList.end()) { // �ִٸ�
			backTrackingList.erase(findPos); // backTrackingList���� ����
		}

		// ��濡 ���� �����ִ��� üũ, ���������� x y ������Ʈ����
		ret = doIBMotion(now, move);
		if (ret == 0) { // ���� ������
			// ���� ����� �� ã��
			Nearest = findNearestPosition();

			// A* �Ἥ �� ���� �� ��� ã��
			fin = astar.findRoute(Nearest.x, Nearest.y, now.x, now.y); // ��� ã��
			//printf("%d\n", astar.hasFindRoute());
			if (astar.hasFindRoute()) {	// �� ã����
				while (fin->parent != nullptr) { // ����� ���κ��� ��� ����
					fin = fin->parent;
					Position tmp(fin->xPos, fin->yPos);
					pathList.push_back(tmp);	// pathList�� ����
					astarRoute.push_back(tmp);	// vector�� ����� ��ε� ����.
				}
				pathList.push_back(Nearest);

				if (show) {
					printf("A* ���� : [%d, %d]\n", now.x, now.y);
					printf("A* ��ǥ : [%d, %d]\n", Nearest.x, Nearest.y);
					for (auto it = astarRoute.begin(); it != astarRoute.end(); ++it) {
						Position tmp = (*it);
						map.setMapData(tmp.x, tmp.y, 200);
						printf("A* ��� ��ǥ : [%d, %d]\n", tmp.x, tmp.y);
					}
					printf("������ ã�� �Ϸ� \n\n");
				}
				// astarRoute �ʱ�ȭ �� ���� ��ǥ ����
				astarRoute.clear();
				now = Nearest;
			}
			else {
				printf("dfdf\n");
				findPos = backTrackingList.find(Nearest);
				if (findPos != backTrackingList.end()) { // �ִٸ�
					backTrackingList.erase(findPos); // backTrackingList���� ����
				}
			}
		}
		else {
			pathList.push_back(now);
		}
	}
	printf("���Ž�� ��! \n");
	if (show) {
		cv::imshow("resultOrigin", resultOrigin);
		cv::waitKey(0);
		cv::destroyAllWindows();
	}

	return pathList;
}


/*
IB(Intellectual Boustrophedon) ����
	�� = x++, 
	�� = x--, 
	�� = y++, 
	�� = y--, 
	�̵��Ұ� ��, return 0
	'��' �� ���·� ��/�� �����ϸ� ���ʿ� ������ ������ ���� �켱 ����
*/
int PathFinder::doIBMotion()
{
	int ret = 1;
	if (map.getMapData(now.x, now.y + move) == LOAD && map.getMapData(now.x - move, now.y) == LOAD) { // �� , ��
		//now.x--;	// ��
		now.x -= move;
		
	}
	else if (map.getMapData(now.x, now.y - move) == LOAD && map.getMapData(now.x - move, now.y) == LOAD) { // ��, ��
		//now.x--;	// ��
		now.x -= move;
	}
	else if (map.getMapData(now.x, now.y - move) == LOAD && map.getMapData(now.x, now.y + move) == LOAD) { // ��, ��
		//now.y++;	// ��
		now.y += move;
	}
	else if (map.getMapData(now.x, now.y - move) == LOAD) { // ��
		//now.y--;	// ��
		now.y -= move;
	}
	else if (map.getMapData(now.x, now.y + move) == LOAD) { // ��
		//now.y++;	// ��
		now.y += move;
	}
	else if (map.getMapData(now.x - move, now.y) == LOAD) { // ��
		//now.x--;	// ��
		now.x -= move;
	}
	else if (map.getMapData(now.x + move, now.y) == LOAD) { // ��
		//now.x++;	// ��
		now.x += move;
	}
	else {
		ret = 0;
	}

	return ret;
}


/* backTrackingList �� �ִ� ��ǥ�� �� now�κ��� ���� ����� �� ã�� */
Position PathFinder::findNearestPosition()
{
	double minDistance = 10000;
	Position Nearest(now.x, now.y);	// ���� ������� ������ ����

	for (auto iter = backTrackingList.begin(); iter != backTrackingList.end(); ++iter) {
		int dist = abs((int)(now.x - (*iter).x)) + abs((int)(now.y - (*iter).y));
		if (dist < minDistance) { // ���� ����� �Ÿ��� ���������� ������
			minDistance = dist; // minDist ������Ʈ
			Nearest.x = (*iter).x;
			Nearest.y = (*iter).y;
		}
	}

	return Nearest;
}
