//#include "mystl_deque.h"
//using std::cout;
//using std::endl;
//
//template<class T, class Alloc, size_t BufSize>
//void print(deque<T, Alloc, BufSize>& v)
//{
//	cout << "deque : ";
//	for (auto e : v)
//		cout << e << " ";
//	cout << endl;
//}
//
//int main()
//{
//
//	deque<int, alloc, 8> dq;
//	dq.push_back(9);
//	dq.push_back(9);
//	dq.push_back(9);
//	
//	for (int i = 1; i <= 9; i++) dq.push_front(i);
//	for (int i = 1; i <= 9; i++) dq.push_back(i);
//	print(dq);
//
//	for (int i = 1; i <= 9; i++) dq.pop_front();
//	for (int i = 1; i <= 9; i++) dq.pop_back();
//	print(dq);
//
//	for (int i = 1; i <= 9; i++) dq.push_front(i);
//	print(dq);
//
//	dq.erase(dq.begin() + 10);
//	dq.insert(dq.begin() + 10, 99);
//	print(dq);
//
//	dq.erase(dq.begin() + 3, dq.begin() + 6);
//	print(dq);
//
//	dq.clear();
//	print(dq);
//
//	return 0;
//}