//#include "mystl_priority_queue.h"
//using std::cout;
//using std::endl;
//
//template<class T>
//class Compare {
//public:
//	Compare(bool ascending = true) : ascending(ascending) {}
//	bool operator()(const T& lhs, const T& rhs) const {
//		return ascending ? lhs > rhs : lhs < rhs;
//	}
//private:
//	bool ascending;
//};
//
//int main() 
//{
//	cout << "大根堆 : " << endl;
//	lth::priority_queue<int> pq;
//	pq.push(5);
//	pq.push(66);
//	pq.push(1);
//	pq.push(1);
//	pq.push(999);
//	pq.push(1);
//	pq.push(44);
//	pq.push(1);
//	pq.push(12);
//	pq.push(1);
//	pq.push(33);
//	pq.push(1);
//	cout << pq.top() << endl;
//
//	while (pq.size())
//	{
//		cout << pq.top() << endl;
//		pq.pop();
//	}
//	cout << "--------------" << endl;
//	cout << "小根堆 : " << endl;
//	lth::priority_queue<int, lth::vector<int>, greater<int>> pq2;
//	pq2.push(5);
//	pq2.push(66);
//	pq2.push(1);
//	pq2.push(1);
//	pq2.push(999);
//	pq2.push(1);
//	pq2.push(44);
//	pq2.push(1);
//	pq2.push(12);
//	pq2.push(1);
//	pq2.push(33);
//	pq2.push(1);
//	cout << pq2.top() << endl;
//
//	while (pq2.size())
//	{
//		cout << pq2.top() << endl;
//		pq2.pop();
//	}
//
//	cout << "--------------" << endl;
//	cout << "小根堆 : " << endl;
//	Compare<int> comp(true);
//	lth::priority_queue<int, lth::vector<int>, Compare<int>> pq3(comp);
//	pq3.push(5);
//	pq3.push(66);
//	pq3.push(1);
//	pq3.push(1);
//	pq3.push(999);
//	pq3.push(1);
//	pq3.push(44);
//	pq3.push(1);
//	pq3.push(12);
//	pq3.push(1);
//	pq3.push(33);
//	pq3.push(1);
//	cout << pq3.top() << endl;
//
//	while (pq3.size())
//	{
//		cout << pq3.top() << endl;
//		pq3.pop();
//	}
//
//	cout << "--------------" << endl;
//	cout << "大根堆 : " << endl;
//	Compare<int> comp2(false);
//	lth::priority_queue<int, lth::vector<int>, Compare<int>> pq4(comp2);
//	pq4.push(5);
//	pq4.push(66);
//	pq4.push(1);
//	pq4.push(1);
//	pq4.push(999);
//	pq4.push(1);
//	pq4.push(44);
//	pq4.push(1);
//	pq4.push(12);
//	pq4.push(1);
//	pq4.push(33);
//	pq4.push(1);
//	cout << pq4.top() << endl;
//
//	while (pq4.size())
//	{
//		cout << pq4.top() << endl;
//		pq4.pop();
//	}
//
//	return 0;
//}