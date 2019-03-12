
#ifndef TINYSTL_H
#define TINYSTL_H

#include <stdio.h>

namespace tinyStl
{
enum NodeColor
{
    RED = 1,
    BLACK = 2
};

/* map 结点 */
struct MapNode
{
    int Key;
    int KeyLength;
    int Value;

    bool Less(MapNode *node)
    {
        if (this->Key < node->Key) return true;
        return false;
    }
};

/* RBTree 结点 */
struct RBNode
{
    RBNode() : Parent(NULL), Left(NULL), Right(NULL) {}

    NodeColor Color;
    MapNode   Data;
    RBNode   *Parent;
    RBNode   *Left;
    RBNode   *Right;
};

/* red-black tree */
struct RBTree
{
    RBTree() : root(NULL) {}

    RBNode *root;

    void Insert(RBNode *node)
    {
        /* root node */
        if (root == NULL)
        {
            root = new RBNode(*node);
            root->Color = BLACK;   // 满足条件 1
            root->Parent = NULL;
        }
        /* child or grandchild of root */
        else
        {
            AddToTree(root, node); // 插入到二叉树中
        }

        /* check */
        Check1(node);
    }

    void AddToTree(RBNode *root, RBNode *node)
    {
        if (node->Data.Less(&root->Data)) // 插入到结点左边
        {
            if (root->Left == NULL) // 终止条件
            {
                root->Left = node;
                node->Parent = root;
                return;
            }

            AddToTree(root->Left, node);
        }
        else // 插入到结点右边
        {
            if (root->Right == NULL) // 终止条件
            {
                root->Right = node;
                node->Parent = root;
                return;
            }

            AddToTree(root->Right, node);
        }
    }

    RBNode *Uncle(RBNode *node)
    {
        RBNode *g = GrandParent(node);
        if (g == NULL) return NULL;
        if (node == g->Left) return g->Right;
        if (node == g->Right) return g->Left;
    }

    RBNode *GrandParent(RBNode *node)
    {
        if ((node != NULL) && (node->Parent != NULL)) return node->Parent->Parent;
        return NULL;

    }

    void Check1(RBNode *node)
    {
        if (node->Parent == NULL) node->Color = BLACK;
        else Check2(node);
    }

    void Check2(RBNode *node)
    {
        if (node->Parent->Color == BLACK) return;
        Check3(node);
    }

    void Check3(RBNode *node)
    {
        RBNode *g, *u = Uncle(node);

        if ((u != NULL) && (u->Color == RED))
        {
            node->Parent->Color = BLACK;
            u->Color = BLACK;

            g = GrandParent(node);
            g->Color = RED;

            Check1(g);
        }
        else
        {
            Check4(node);
        }
    }

    void Check4(RBNode *node)
    {
        RBNode *g = GrandParent(node);

        if ((node == node->Parent->Right) && (node->Parent == g->Left))
        {
            RotateLeft(node->Parent);
            node = node->Left;
        }
        else if ((node == node->Parent->Left) && (node->Parent == g->Right))
        {
            RotateRight(node->Parent);
            node = node->Right;
        }
    }

    void Check5(RBNode *node)
    {
        RBNode * g = GrandParent(node);
        node->Parent->Color = BLACK;
        g->Color = RED;

        if (node == node->Parent->Left) RotateRight(node);
        else RotateLeft(node);
    }

    void RotateLeft(RBNode *node)
    {
        RBNode *right = node->Right;
        right->Parent = node->Parent;
        node->Parent = right;

        node->Right = right->Left;
        right->Left = node;

    }
    void RotateRight(RBNode *node)
    {
        RBNode *left = node->Left;
        left->Parent = node->Parent;
        node->Parent = left;

        node->Left = left->Right;
        left->Right = node;

    }
};

class tinyMap
{
public:
    void insert(int key, int keyLength, int value)
    {
        RBNode *node = new RBNode();
        node->Data.Key = key;
        node->Data.KeyLength = keyLength;
        node->Data.Value = value;
        rbTree.Insert(node);
    }

    RBNode *find(int key)
    {
        RBNode * node = rbTree.root;
        while ((node != NULL) && (node->Data.Key !=key))
        {
            if (key < node->Data.Key) node = node->Left;
            else node = node->Right;
        }

        return node;
    }

    RBNode *find(int key, int keyLength)
    {
        RBNode *node = find(key);
        if (node != NULL)
            return (node->Data.KeyLength == keyLength ? node : NULL);
        return NULL;
    }

    int operator[] (int key)
    {
        return find(key)->Data.Value;
    }

    RBNode *end()
    {
        return NULL;
    }

private:
    RBTree rbTree;
};
}


#endif // TINYSTL_H
