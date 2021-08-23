#ifndef _KD_TREE_H	
#define _KD_TREE_H

#include <set>
#include <array>
#include <queue>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <initializer_list>

using namespace std;

/***************************************************/
/*												   */
/* 				 (C) Uri Raz, 2015				   */
/*				uri.raz@private.org.il			   */
/*												   */
/* Shared for the greater glory of the C.P.O.N.O.D */
/*												   */
/***************************************************/

template <unsigned int K, class T> class kd_tree_iterator;

template <unsigned int K, class T>
class kd_tree
{
public:

	//-------------------------------------------------------------------------------------

	class kd_point
	{
	private:
		array<T, K> Coords;
		int id; // added by Hao

	public:
		kd_point() { Coords.fill(0.0f); id = 0; }

		kd_point(initializer_list<T> l)
		{
			unsigned i = 0;

			for (initializer_list<T>::iterator It = l.begin(); It < l.end(); It++, i++)
				Coords[i] = *It;
		}

		kd_point(initializer_list<T> l, int p)
		{
			unsigned i = 0;

			for (initializer_list<T>::iterator It = l.begin(); It < l.end(); It++, i++)
				Coords[i] = *It;

			id = p;
		}

		void fill(T val) { Coords.fill(val); }

		T& operator[](std::size_t idx) { return Coords[idx]; }
		const T& operator[](std::size_t idx) const { return const_cast<T&>(Coords[idx]); }

		bool operator==(const kd_point& rhs) const
		{
			bool retVal = true;

			for (unsigned i = 0; i < K && retVal; i++)
				if (Coords[i] != rhs.Coords[i])
					retVal = false;

			return retVal;
		}

		bool operator!=(const kd_point& rhs) const { return !(*this == rhs); }

		bool operator<(const kd_point& rhs) const
		{
			bool retVal = false;

			for (unsigned i = 0; i < K; i++)
				if (Coords[i] < rhs.Coords[i])
				{
					retVal = true;
					break;
				}
				else if (Coords[i] > rhs.Coords[i])
					break;

			return retVal;
		}

		bool isValid() const
		{
			for (unsigned i = 0; i < K; i++)
				if ( isnan(Coords[i]) ||
					 Coords[i] ==  numeric_limits<T>::infinity() ||
					 Coords[i] == -numeric_limits<T>::infinity() )
				  return false;

			return true;
		}

		inline int getid() const { return id; } // added by Hao
		inline void setid(int p) { id = p; } // added by Hao
	};	// kd_point

	//-----------------------------------------------------------------------------------

protected:
	// Node class - internal to KD tree
	class kd_node
	{
	public:
		virtual bool isInternal() = 0;
	};

	class kd_internal_node : public kd_node
	{
	private:
		T m_splitVal;

	public:
		virtual bool isInternal() override { return true; }
		T splitVal() { return m_splitVal; }

		shared_ptr<kd_node> m_Left, m_Right;

		kd_internal_node(const T splitVal) : m_splitVal(splitVal) { }
	};

	class kd_leaf_node : public kd_node
	{
	public:
		virtual bool isInternal() override { return false; }

		kd_point m_Vals;
		weak_ptr<kd_leaf_node> m_Next, m_Prev;

		kd_leaf_node(const kd_point &Points) : m_Vals(Points) { }
	};

	// -----------------------------------------------------------------

	shared_ptr<kd_node> m_Root;

	using kd_Box = pair<kd_point, kd_point>;

	// ----------------------------------------------------------------------------------------
	//
	// This function is implemented in a naive & inefficient way.
	//
	void FindNearestNeighbor(const kd_point &srcPoint, kd_point &nearPoint,
							 T &minDistance, kd_Box &minRegion, kd_Box &currRegion,
							 shared_ptr<kd_node> currNode, const unsigned int Depth = 0) const
	{
		if (currNode->isInternal())
		{
			shared_ptr<kd_internal_node> Node = dynamic_pointer_cast<kd_internal_node>(currNode);

			kd_Box lSubRegion(currRegion), rSubRegion(currRegion);

			lSubRegion.second[Depth%K] = rSubRegion.first[Depth%K] = Node->splitVal();

			if (Node->m_Left != nullptr && regionCrossesRegion(lSubRegion, minRegion))
				FindNearestNeighbor(srcPoint, nearPoint, minDistance, minRegion, lSubRegion, Node->m_Left, Depth + 1);

			if (Node->m_Right != nullptr && regionCrossesRegion(rSubRegion, minRegion))
				FindNearestNeighbor(srcPoint, nearPoint, minDistance, minRegion, rSubRegion, Node->m_Right, Depth + 1);
		}
		else
		{
			shared_ptr<kd_leaf_node> Node = dynamic_pointer_cast<kd_leaf_node>(currNode);

			if (Distance(srcPoint, Node->m_Vals) <= minDistance)
			{
				nearPoint = Node->m_Vals;
				minDistance = Distance(srcPoint, nearPoint);

				for (unsigned index = 0; index < K; index++)
				{
					minRegion.first[index] = srcPoint[index] - minDistance;
					minRegion.second[index] = srcPoint[index] + minDistance;
				}
			}
		}
	}

	// ---------------------------------------------------------------------------------------------------

	void FindKNearestNeighbors(const kd_point &srcPoint, vector<kd_point> &nearPoints, const unsigned k,
							   T &minDistance, kd_Box &minRegion, kd_Box &currRegion,
							   shared_ptr<kd_node> currNode, const unsigned int Depth = 0) const
	{
		static set<kd_point> nearSet;

		if (Depth == 0)
		{
			nearSet.clear();

			for (size_t i = 0; i < nearPoints.size() && nearPoints[i].isValid(); i++)
				nearSet.insert(nearPoints[i]);
		}


		if (currNode->isInternal())
		{
			shared_ptr<kd_internal_node> Node = dynamic_pointer_cast<kd_internal_node>(currNode);

			kd_Box lSubRegion(currRegion), rSubRegion(currRegion);

			lSubRegion.second[Depth%K] = rSubRegion.first[Depth%K] = Node->splitVal();

			if (Node->m_Left != nullptr && regionCrossesRegion(lSubRegion, minRegion))
				FindKNearestNeighbors(srcPoint, nearPoints, k, minDistance, minRegion, lSubRegion, Node->m_Left, Depth + 1);

			if (Node->m_Right != nullptr && regionCrossesRegion(rSubRegion, minRegion))
				FindKNearestNeighbors(srcPoint, nearPoints, k, minDistance, minRegion, rSubRegion, Node->m_Right, Depth + 1);
		}
		else
		{
			shared_ptr<kd_leaf_node> Node = dynamic_pointer_cast<kd_leaf_node>(currNode);

			kd_point currPoint = Node->m_Vals;

			if (Distance(srcPoint, currPoint) <= minDistance && nearSet.find(currPoint) == nearSet.end())
			{
				nearSet.erase(nearPoints[k - 1]);
				nearSet.insert(currPoint);

				nearPoints[k - 1] = currPoint;

				for (unsigned i = k - 1; i > 0; i--)
					if (Distance(srcPoint, nearPoints[i-1]) > Distance(srcPoint, nearPoints[i]))
						swap(nearPoints[i-1], nearPoints[i]);
					else
						break;

				minDistance = Distance(srcPoint, nearPoints[k-1]);

				for (unsigned index = 0; index < K; index++)
				{
					minRegion.first[index]  = srcPoint[index] - minDistance;
					minRegion.second[index] = srcPoint[index] + minDistance;
				}
			}
		}
	}

	// ------------------------------------------------------------------------------

	// The routine has a desired side effect of sorting Points
	T NthCoordMedian(vector<kd_point> &Points, const unsigned num)
	{
		sort(Points.begin(), Points.end(), [num](kd_point &A, kd_point &B) { return A[num%K] < B[num%K]; });

		T Median = Points[Points.size() / 2][num];

		if (Points.size() % 2 == 0)
			Median = (Median + Points[Points.size() / 2 - 1][num]) / 2.0f;

		if (Median == Points[Points.size() - 1][num] &&
			Median != Points[0][num])
		{
			int index = Points.size() / 2;

			while (Median == Points[--index][num]);

			Median = (Median + Points[index][num]) / 2.0f;
		}

		return Median;
	}

	// ------------------------------------------------------------------------------------------------------------------------

	shared_ptr<kd_node> CreateTree(vector<kd_point> &Points, shared_ptr<kd_leaf_node> &Last_Leaf, const unsigned int Depth = 0)
	{
		if (Points.size() == 1)
		{
			shared_ptr<kd_leaf_node> retNode(new kd_leaf_node(Points[0]));

			if (Last_Leaf)
			{
				Last_Leaf->m_Next = retNode;
				retNode->m_Prev = Last_Leaf;
			}

			Last_Leaf = retNode;

			return retNode;
		}
		else if (Points.size() == 2)
		{
			if (Points[0][Depth%K] == Points[1][Depth%K])
			{
				shared_ptr<kd_internal_node> retNode(new kd_internal_node(Points[0][Depth%K]));

				retNode->m_Left = CreateTree(Points, Last_Leaf, Depth + 1);

				return retNode;
			}
			else
			{
				if (Points[0][Depth%K] > Points[1][Depth%K])
					swap(Points[0], Points[1]);

				shared_ptr<kd_internal_node> retNode(new kd_internal_node((Points[0][Depth%K] + Points[1][Depth%K]) / 2.0f));
				shared_ptr<kd_leaf_node> Left(new kd_leaf_node(Points[0])),
										 Right(new kd_leaf_node(Points[1]));
				if (Last_Leaf)
				{
					Last_Leaf->m_Next = Left;
					Left->m_Prev = Last_Leaf;
				}

				Left->m_Next = Right;
				Right->m_Prev = Left;

				retNode->m_Left  = Left;
				retNode->m_Right = Right;

				Last_Leaf = Right;

				return retNode;
			}
		}
		else
		{
			T Median = NthCoordMedian(Points, Depth%K);

			shared_ptr<kd_internal_node> retNode(new kd_internal_node(Median));

			vector<kd_point> subtreePoints;

			subtreePoints.reserve(Points.size()/2);

			for (size_t index = 0; index < Points.size() && Points[index][Depth%K] <= Median; index++)
				subtreePoints.push_back(Points.at(index));

			if (subtreePoints.size() > 0)
				retNode->m_Left = CreateTree(subtreePoints, Last_Leaf, Depth + 1);

			unsigned int insertedPoints = subtreePoints.size();
			unsigned int remainedPoints = Points.size() - subtreePoints.size();

			subtreePoints.clear();
			subtreePoints.reserve(remainedPoints);

			for (size_t index = insertedPoints; index < Points.size(); index++)
				subtreePoints.push_back(Points.at(index));

			if (subtreePoints.size() > 0)
				retNode->m_Right = CreateTree(subtreePoints, Last_Leaf, Depth + 1);

			subtreePoints.clear();

			return retNode;
		}
	}
	
	// ---------------------------------------------------------------------------------------

	void SearchKdTree(const kd_Box &searchBox, const kd_Box &region, vector<kd_point> &Points,
					  shared_ptr<kd_node> currNode, const unsigned Depth = 0) const
	{
		if (!currNode->isInternal())
		{
			shared_ptr<kd_leaf_node> Node = dynamic_pointer_cast<kd_leaf_node>(currNode);

			if (pointIsInRegion(Node->m_Vals, searchBox))
				Points.push_back(Node->m_Vals);
		}
		else
		{
			shared_ptr<kd_internal_node> Node = dynamic_pointer_cast<kd_internal_node>(currNode);

			kd_Box lSubRegion(region), rSubRegion(region);

			lSubRegion.second[Depth%K] = rSubRegion.first[Depth%K] = Node->splitVal();

			if (Node->m_Left != nullptr && regionCrossesRegion(searchBox, lSubRegion))
				SearchKdTree(searchBox, lSubRegion, Points, Node->m_Left, Depth + 1);

			if (Node->m_Right != nullptr && regionCrossesRegion(searchBox, rSubRegion))
				SearchKdTree(searchBox, rSubRegion, Points, Node->m_Right, Depth + 1);
		}
	}

	// -------------------------------------------------------------------------------

	shared_ptr<kd_leaf_node> ApproxNearestNeighborNode(const kd_point &srcPoint) const
	{
		unsigned int Depth = 0;
		shared_ptr<kd_node> Node(m_Root);

		while (Node->isInternal())
		{
			shared_ptr<kd_internal_node> iNode = dynamic_pointer_cast<kd_internal_node>(Node);

			if (srcPoint[Depth++%K] <= iNode->splitVal() || iNode->m_Right == nullptr)
				Node = iNode->m_Left;
			else
				Node = iNode->m_Right;
		}

		shared_ptr<kd_leaf_node> lNode = dynamic_pointer_cast<kd_leaf_node>(Node);

		return lNode;
	}

	// ----------------------------------------------------------------

	T ApproxNearestNeighborDistance(const kd_point &srcPoint) const
	{
		shared_ptr<kd_leaf_node> node = ApproxNearestNeighborNode(srcPoint);

		return Distance(srcPoint, node->m_Vals);
	}

	// ------------------------------------------------

	unsigned TreeHeight(shared_ptr<kd_node> node) const
	{
		if (node == nullptr) return 0;

		if (node->isInternal())
		{
			shared_ptr<kd_internal_node> iNode = dynamic_pointer_cast<kd_internal_node>(node);

			return 1 + max(TreeHeight(iNode->m_Left), TreeHeight(iNode->m_Right));
		}
		else
			return 1;
	}

	// ---------------------------------------------------------------

	void PrintTree(shared_ptr<kd_node> node, unsigned int depth) const
	{
		for (unsigned i = 0; i < depth; i++)
			cout << " ";

		if (node == nullptr)
			cout << "null" << endl;
		else
		{
			if (node->isInternal())
			{
				shared_ptr<kd_internal_node> iNode = dynamic_pointer_cast<kd_internal_node>(node);

				cout << "Split val is " << iNode->splitVal() << " for axis #" << depth%K + 1 << endl;

				PrintTree(iNode->m_Left, depth + 1);
				PrintTree(iNode->m_Right, depth + 1);
			}
			else
			{
				shared_ptr<kd_leaf_node> lNode = dynamic_pointer_cast<kd_leaf_node>(node);

				cout << "Point is (";

				for (unsigned i = 0; i < K; i++)
					cout << lNode->m_Vals[i] << " ";

				cout << ")" << endl;
			}
		}
	}

public:
	void clear() { m_Root.reset(); }

	bool operator==(const kd_tree<K, T> rhs) = delete;
	bool operator=(const kd_tree<K, T> rhs) = delete;

	friend class kd_tree_iterator<K, T>;
	typedef typename kd_tree_iterator<K, T> iterator;

	kd_tree_iterator<K, T> end();
	kd_tree_iterator<K, T> begin();

	kd_tree() { }

	kd_tree(vector<kd_point> &Points) { insert(Points); }

	static bool pointIsInRegion(const kd_point &Point, const pair<kd_point, kd_point> &Region);
	static bool regionCrossesRegion(const pair<kd_point, kd_point> &Region1, const pair<kd_point, kd_point> &Region2);

	static T Distance(const kd_point &P, const kd_point &Q);

	// ----------------------------------

	void insert(vector<kd_point> &Points)
	{
		clear();

		for (signed i = Points.size() - 1; i >= 0; i--)
			if (!Points[i].isValid())
				Points.erase(Points.begin() + i);

		if (Points.size() > 0)
		{
			sort(Points.begin(), Points.end());
			vector<kd_point>::iterator it = unique(Points.begin(), Points.end());
			Points.resize(distance(Points.begin(), it));

			shared_ptr<kd_leaf_node> dummyLeaf;

			m_Root = CreateTree(Points, dummyLeaf);
		}
	}

	void insert_without_sort(vector<kd_point> &Points)
	{
		clear();

		for (signed i = Points.size() - 1; i >= 0; i--)
			if (!Points[i].isValid())
				Points.erase(Points.begin() + i);

		if (Points.size() > 0)
		{
			//sort(Points.begin(), Points.end());
			vector<kd_point>::iterator it = unique(Points.begin(), Points.end());
			Points.resize(distance(Points.begin(), it));

			shared_ptr<kd_leaf_node> dummyLeaf;

			m_Root = CreateTree(Points, dummyLeaf);
		}
	}

	// --------------------------------------------------------------------------------------------

	void search(const kd_point &minPoint, const kd_point &maxPoint, vector<kd_point> &Points) const
	{
		Points.clear();

		if (m_Root == nullptr)
			return;

		kd_Box region, sorted;

		region.first.fill(-numeric_limits<T>::infinity());
		region.second.fill(numeric_limits<T>::infinity());

		for (unsigned coord = 0; coord < K; coord++)
		{
			sorted.first[coord]  = min(minPoint[coord], maxPoint[coord]);
			sorted.second[coord] = max(minPoint[coord], maxPoint[coord]);
		}

		SearchKdTree(sorted, region, Points, m_Root);
	}

	// --------------------------------------------------------------------------

	bool FindNearestNeighbor(const kd_point &srcPoint, kd_point &nearPoint) const
	{
		bool retVal = (m_Root != nullptr);

		if (!m_Root)
			nearPoint.fill(numeric_limits<T>::quiet_NaN());
		else
		{
			T minDistance = ApproxNearestNeighborDistance(srcPoint);

			kd_Box minBox, maxBox;

			maxBox.first.fill(-numeric_limits<T>::infinity());
			maxBox.second.fill(numeric_limits<T>::infinity());

			for (unsigned coord = 0; coord < K; coord++)
			{
				minBox.first[coord] = srcPoint[coord] - minDistance;
				minBox.second[coord] = srcPoint[coord] + minDistance;
			}

			FindNearestNeighbor(srcPoint, nearPoint, minDistance, minBox, maxBox, m_Root);
		}

		return retVal;
	}

	// -------------------------------------------------------------------------------------------------------

	bool FindKNearestNeighbors(const kd_point &srcPoint, vector<kd_point> &nearPoints, const unsigned k) const
	{
		nearPoints.clear();

		if (!m_Root) return false;

		shared_ptr<kd_leaf_node> nNode = ApproxNearestNeighborNode(srcPoint),
								 pNode = nNode->m_Prev.lock();

		nearPoints.push_back(nNode->m_Vals);

		nNode = nNode->m_Next.lock();

		while (nearPoints.size() < k && ( nNode || pNode ))
		{
			if (nNode)
			{
				nearPoints.push_back(nNode->m_Vals);

				nNode = nNode->m_Next.lock();
			}

			if (pNode && nearPoints.size() < k)
			{
				nearPoints.push_back(pNode->m_Vals);

				pNode = pNode->m_Prev.lock();
			}
		}

		sort(nearPoints.begin(), nearPoints.end(),
			 [srcPoint](kd_point &A, kd_point &B) {return Distance(srcPoint, A) < Distance(srcPoint, B); });

		T minDistance;

		if (nearPoints.size() < k)
		{
			kd_point infinityPoint;
			infinityPoint.fill(numeric_limits<T>::infinity());

			nearPoints.resize(k, infinityPoint);
			
			minDistance = numeric_limits<T>::infinity();
		}
		else
			minDistance = Distance(srcPoint,nearPoints[k - 1]);

		kd_Box minBox, maxBox;

		for (unsigned i = 0; i < K; i++)
		{
			maxBox.first[i]  = srcPoint[i] - minDistance;
			maxBox.second[i] = srcPoint[i] + minDistance;
		}

		minBox = maxBox;

		FindKNearestNeighbors(srcPoint, nearPoints, k, minDistance, minBox, maxBox, m_Root);

		for (signed i = k - 1; i > 0 && !nearPoints[i].isValid(); i--)
			nearPoints.erase(nearPoints.begin() + i);

		return true;
	}

	// -----------------------------------------------------

	unsigned nodeCount(bool withInternalNodes = false) const
	{
		if (m_Root == nullptr) return 0;

		unsigned Count = 0;
		shared_ptr<kd_node> Node;
		queue<shared_ptr<kd_node>> Nodes;
		 
		Nodes.push(m_Root);

		while (!Nodes.empty())
		{
			Node = Nodes.front(); Nodes.pop();

			if (Node->isInternal())
			{
				if (withInternalNodes)
					Count++;

				shared_ptr<kd_internal_node> iNode = dynamic_pointer_cast<kd_internal_node>(Node);

				if (iNode->m_Left) Nodes.push(iNode->m_Left);
				if (iNode->m_Right) Nodes.push(iNode->m_Right);
			}
			else
				Count++;
		}

		return Count;
	}

	// -------------------------------------------------------------

	void PrintTree() const { PrintTree(m_Root, 0); }

	unsigned TreeHeight() const { return TreeHeight(this->m_Root); }
};

// ---------------------------------------------------------------------

template <unsigned int K, class T>
T kd_tree<K, T>::Distance(const kd_point &P, const kd_point &Q)
{
	T Sum = 0;

	for (unsigned i = 0; i < K; i++)
		Sum += (P[i] - Q[i]) * (P[i] - Q[i]);

	return sqrt(Sum);
}

// ----------------------------------------------------------------------------------------------------

template <unsigned int K, class T>
bool kd_tree<K, T>::pointIsInRegion(const kd_point &Point, const pair<kd_point, kd_point> &Region)
{
	bool isInRegion = true;

	for (unsigned i = 0; i < K && isInRegion; i++)
		if (!(Region.first[i] <= Point[i] && Point[i] <= Region.second[i]))
			isInRegion = false;

	return isInRegion;
}

// ----------------------------------------------------------------------------------

template <unsigned int K, class T>
bool kd_tree<K, T>::regionCrossesRegion(const pair<kd_point, kd_point> &Region1,
									 const pair<kd_point, kd_point> &Region2)
{
	bool regionsCross = true;

	for (unsigned i = 0; i < K && regionsCross; i++)
		if (Region1.first[i] > Region2.second[i] || Region1.second[i] < Region2.first[i])
			regionsCross = false;

	return regionsCross;
}

// -----------------------
//
// Iterator implementation
//
template <unsigned int K, class T>
class kd_tree_iterator : public std::iterator<output_iterator_tag, void, void, void, void>
{
public:
	kd_tree_iterator() {}
	kd_tree_iterator(shared_ptr<typename kd_tree<K, T>::kd_node> node) : nodePtr(node) {}

	template <unsigned int K, class T> friend bool operator== (const kd_tree_iterator<K, T>& lhs, const kd_tree_iterator<K, T>& rhs);
	template <unsigned int K, class T> friend bool operator!= (const kd_tree_iterator<K, T>& lhs, const kd_tree_iterator<K, T>& rhs);

	typename kd_tree<K, T>::kd_point& operator*()
	{
		shared_ptr<typename kd_tree<K, T>::kd_node> node = this->nodePtr;

		shared_ptr<kd_tree<K, T>::kd_leaf_node> lNode = dynamic_pointer_cast<kd_tree<K, T>::kd_leaf_node>(node);

		return lNode->m_Vals;
	}

	kd_tree_iterator& operator++()
	{
		shared_ptr<kd_tree<K, T>::kd_leaf_node> lNode =
			dynamic_pointer_cast<kd_tree<K, T>::kd_leaf_node>(this->nodePtr);

		weak_ptr<kd_tree<K, T>::kd_leaf_node> lNext = lNode->m_Next;

		this->nodePtr = dynamic_pointer_cast<kd_tree<K, T>::kd_leaf_node>(lNext.lock());

		return *this;
	}

	kd_tree_iterator operator++(int)
	{
		kd_tree_iterator tmp(*this);
		operator++();
		return tmp;
	}

private:
	shared_ptr<typename kd_tree<K, T>::kd_node> nodePtr;
};

// -----------------------------------------------------------------------------

template <unsigned int K, class T>
bool operator== (const kd_tree_iterator<K, T>& lhs, const kd_tree_iterator<K, T>& rhs)
{
	return (lhs.nodePtr == rhs.nodePtr);
}

// -----------------------------------------------------------------------------

template <unsigned int K, class T>
bool operator!= (const kd_tree_iterator<K, T> &lhs, const kd_tree_iterator<K, T>& rhs)
{
	return !(lhs == rhs);
}

// -----------------------------------

template <unsigned int K, class T>
kd_tree_iterator<K, T> kd_tree<K, T>::end()
{
	kd_tree<K, T>::iterator retVal;

	return retVal;
}

// -------------------------------------

template <unsigned int K, class T>
kd_tree_iterator<K, T> kd_tree<K, T>::begin()
{
	if (!this->m_Root) return end();

	shared_ptr<kd_tree<K, T>::kd_node> node = this->m_Root;

	while (node->isInternal())
	{
		shared_ptr<kd_internal_node> iNode = dynamic_pointer_cast<kd_internal_node>(node);

		node = iNode->m_Left;
	}

	kd_tree<K, T>::iterator retVal(node);

	return retVal;
}

#endif // _KD_TREE_H