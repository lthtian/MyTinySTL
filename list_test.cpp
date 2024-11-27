//#include "mystl_list.h"
//using std::cout;
//using std::endl;
//
//template<class t>
//void print(list<t>& v)
//{
//	cout << "l : ";
//	for (auto e : v)
//		cout << e << " ";
//	cout << endl;
//}
//
//int main()
//{
//	list<int> l;
//
//	l.push_back(1);
//	l.push_back(1);
//	l.push_back(1);
//	for (int i = 1; i <= 10; i++) l.push_back(i);
//	cout << l.front() << endl;
//	cout << l.back() << endl;
//	print(l);
//
//	l.insert(++l.begin(), 999);
//	print(l);
//
//	l.erase(--l.end());
//	print(l);
//
//	l.erase(--(--l.end()));
//	print(l);
//
//	l.remove(999);
//	print(l);
//
//	l.unique();
//	print(l);
//	
//	l.reverse();
//	print(l);
//
//	list<int> l2;
//	l2.push_back(5);
//	l2.push_back(4);
//	l2.push_back(3);
//	l2.push_back(1);
//	l2.push_back(5);
//	print(l2);
//	
//	l2.sort(); 
//	print(l2);
//
//	// ¶ÔstdËã·¨µÄ¼æÈİ
//	for (int i = 0; i <= 7; i++)
//	{
//		__list_iterator<int, int&, int*> it = std::find(l2.begin(), l2.end(), i);
//		if (it == l2.end()) cout << "notfind " << i << endl;
//		else cout << "find " << i << endl;
//	}
//
//	return 0;
//}