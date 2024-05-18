//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Adam Zvara <xzvara01@stud.fit.vutbr.cz>
// $Date:       $2021-02-21
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Adam Zvara
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>
#include "gtest/gtest.h"
#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

class EmptyTree : public ::testing::Test
{
    protected:
        BinaryTree emptyTree;
};

TEST_F(EmptyTree, IsEmpty)
{
    std::vector<BinaryTree::Node_t *>nodes;
    emptyTree.GetAllNodes(nodes);

    EXPECT_TRUE(nodes.empty());
}

TEST_F(EmptyTree, InsertNode)
{
    int value = 5;
    std::pair<bool, BinaryTree::Node_t *> pair = emptyTree.InsertNode(value);

    EXPECT_TRUE(pair.first);
    EXPECT_NE(pair.second, nullptr);

    BinaryTree::Node_t *root = emptyTree.GetRoot();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->key, value);
    EXPECT_EQ(root->pParent, nullptr);
    EXPECT_EQ(root->color, BinaryTree::BLACK);
    
    BinaryTree::Node_t *right = root->pRight;
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->pRight, nullptr);
    EXPECT_EQ(right->pLeft, nullptr);
    EXPECT_NE(right->pParent, nullptr);
    EXPECT_EQ(right->color, BinaryTree::BLACK);
     
    BinaryTree::Node_t *left = root->pLeft;
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->pRight, nullptr);
    EXPECT_EQ(left->pLeft, nullptr);
    EXPECT_NE(left->pParent, nullptr);
    EXPECT_EQ(left->color, BinaryTree::BLACK);
}

TEST_F(EmptyTree, DeleteNode)
{
    EXPECT_FALSE(emptyTree.DeleteNode(1));
    EXPECT_FALSE(emptyTree.DeleteNode(100));
    EXPECT_EQ(emptyTree.GetRoot(), nullptr);
}

TEST_F(EmptyTree, FindNode)
{
    EXPECT_EQ(emptyTree.FindNode(1), nullptr);
    EXPECT_EQ(emptyTree.FindNode(100), nullptr);
    EXPECT_EQ(emptyTree.GetRoot(), nullptr);
}

class NonEmptyTree : public ::testing::Test
{
    protected:
        
        BinaryTree tree;
        
        //Vytvorenie testovacieho stromu
        /*               3
         *       1             7
         *                5         12
         *             4         9     28
         */               
        int ammount = 8,
            nodes[8] = {1,3,7,12,5,4,9,28};
        
        void SetUp() override 
        {
            for (int value : nodes) 
                tree.InsertNode(value);
        }
};

TEST_F(NonEmptyTree, InsertNode_New)
{
    /*Priadnie so zmenou struktury stromu*/
    int value = 8;
    std::pair<bool, BinaryTree::Node_t *> pair = tree.InsertNode(value);

    //kontrola vlastnosti prvku
    EXPECT_EQ(pair.first, true);
    BinaryTree::Node_t *new_node = pair.second;
    ASSERT_NE(new_node, nullptr);
    EXPECT_EQ(new_node->key, value);
    EXPECT_EQ(new_node->color, BinaryTree::RED);

    //kontrola rodica
    BinaryTree::Node_t *parent = new_node->pParent;
    ASSERT_NE(parent, nullptr);
    EXPECT_EQ(parent->key, 9);
    EXPECT_EQ(parent->color, BinaryTree::BLACK);

    //kontrola zmeny korena
    BinaryTree::Node_t *root = tree.GetRoot();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->key, 7);
    EXPECT_EQ(root->color, BinaryTree::BLACK);

    //kontrola zmeny potomkov korena
    ASSERT_NE(root->pRight, nullptr);
    EXPECT_EQ(root->pRight->key, 12);
    EXPECT_EQ(root->pRight->color, BinaryTree::RED);
    ASSERT_NE(root->pLeft, nullptr);
    EXPECT_EQ(root->pLeft->key, 3);
    EXPECT_EQ(root->pLeft->color, BinaryTree::RED);

    /*Pridanie bez zmeny struktury stromu*/
    value = 11;
    pair = tree.InsertNode(value);

    //kontrola vlastnosti prvku
    EXPECT_TRUE(pair.first);
    new_node = pair.second;
    ASSERT_NE(new_node, nullptr);
    EXPECT_EQ(new_node->key, value);
    EXPECT_EQ(new_node->color, BinaryTree::RED);

    //kontrola rodica
    parent = new_node->pParent;
    ASSERT_NE(parent, nullptr);
    EXPECT_EQ(parent->key, 9);
    EXPECT_EQ(parent->color, BinaryTree::BLACK);
}

TEST_F(NonEmptyTree, InsertNode_Existing)
{
    int value = 5;
    std::pair<bool, BinaryTree::Node_t *> pair = tree.InsertNode(value);

    //kontrola vlastnosti prvku
    EXPECT_EQ(pair.first, false);
    BinaryTree::Node_t *existing_node = pair.second;
    ASSERT_NE(existing_node, nullptr);
    EXPECT_EQ(existing_node->key, value);
    EXPECT_EQ(existing_node->color, BinaryTree::BLACK);

    //kontrola rodica 
    BinaryTree::Node_t *parent = existing_node->pParent;
    ASSERT_NE(parent, nullptr);
    EXPECT_EQ(parent->key, 7); 
    EXPECT_EQ(parent->color, BinaryTree::RED);

    //kontrola potomka vlavo
    BinaryTree::Node_t *left_node = existing_node->pLeft;
    ASSERT_NE(left_node, nullptr);
    EXPECT_EQ(left_node->key, 4);
    EXPECT_EQ(left_node->color, BinaryTree::RED);
}

TEST_F(NonEmptyTree, DeleteNode_NonExistant)
{
    EXPECT_FALSE(tree.DeleteNode(100));
    EXPECT_FALSE(tree.DeleteNode(0));
}

TEST_F(NonEmptyTree, DeleteNode_Existing)
{
    /*Zmazanie prvku so zmenou struktury*/
    int value = 3;
    EXPECT_TRUE(tree.DeleteNode(value));

    //Kontrola zmeny korena
    BinaryTree::Node_t *root = tree.GetRoot();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->key,7);
    EXPECT_EQ(root->color, BinaryTree::BLACK);

    //Kontrola potomkov korena
    BinaryTree::Node_t *left = root->pLeft; 
    BinaryTree::Node_t *right = root->pRight;

    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->key, 4);
    EXPECT_EQ(left->color, BinaryTree::RED);

    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->key, 12);
    EXPECT_EQ(right->color, BinaryTree::BLACK);
    
    /*Zmazanie prvku bez zmeny struktury*/
    value = 1;
    EXPECT_TRUE(tree.DeleteNode(value));

    //Kontrola povodneho rodica
    BinaryTree::Node_t * parent = tree.GetRoot()->pLeft;
    ASSERT_NE(parent, nullptr);
    EXPECT_EQ(parent->key, 4);
    EXPECT_EQ(parent->color, BinaryTree::BLACK);

    //Kontrola povodneho surodenca
    BinaryTree::Node_t * sibling = parent->pRight;
    ASSERT_NE(sibling, nullptr);
    EXPECT_EQ(sibling->key, 5);
    EXPECT_EQ(sibling->color, BinaryTree::RED);
}

TEST_F(NonEmptyTree, DeleteNode_All)
{
    for (int i : nodes)
    {
        EXPECT_TRUE(tree.DeleteNode(i));
    }

    EXPECT_EQ(tree.GetRoot(), nullptr);
}

TEST_F(NonEmptyTree, FindNode)
{
    int value = -5;
    BinaryTree::Node_t *node = tree.FindNode(value);
    ASSERT_EQ(node, nullptr);

    value = 3;
    node = tree.FindNode(value);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node, tree.GetRoot());
}

/*V triede moze byt vytvoreny akykolvek iny strom, testy na axiomy
 * nie su viazane na konkretnu strukturu
 */
class TreeAxioms : public ::testing::Test
{
    protected:
        BinaryTree axiomTree;
        
        int ammount = 10,
            nodes[10] = {1,3,7,9,15,80,60,42,10,50};
        
        void SetUp() override 
        {
            for (int value : nodes) 
                axiomTree.InsertNode(value);
        }
};

//Vsetky listy su cierne
TEST_F(TreeAxioms, Axiom1)
{
    std::vector<BinaryTree::Node_t *> leafNodes;
    axiomTree.GetLeafNodes(leafNodes);

    for (BinaryTree::Node_t *node : leafNodes)
        EXPECT_EQ(node->color, BinaryTree::BLACK);
}

//Uzel je cerveny - potomci su cierny
TEST_F(TreeAxioms, Axiom2)
{
    std::vector<BinaryTree::Node_t *> nodes;
    axiomTree.GetNonLeafNodes(nodes);

    for (BinaryTree::Node_t *node : nodes)
    {
        if (node->color == BinaryTree::RED)
        {
            EXPECT_EQ(node->pRight->color, BinaryTree::BLACK);
            EXPECT_EQ(node->pLeft->color, BinaryTree::BLACK);
        }
    }
}

//Pomocna funkcia na zistenie poctu ciernych uzlov pri prechode od listu po koren
int CountBlackNodes(BinaryTree::Node_t * node, BinaryTree::Node_t * root)
{
    int count = 0;
    BinaryTree::Node_t *tmp = node;

    while (tmp->pParent != root)
    {
        tmp = tmp->pParent;
        if (tmp->color == BinaryTree::BLACK)
            count++;
    }

    return count;
}

//Rovnaky pocet ciernych uzlov pri prechode listy->koren
TEST_F(TreeAxioms, Axiom3)
{
    std::vector<BinaryTree::Node_t *> leafNodes;
    axiomTree.GetLeafNodes(leafNodes);

    //vypocet poctu ciernych uzlov od prveho listu
    int reference = CountBlackNodes(leafNodes[0], axiomTree.GetRoot());
    
    int actual = 0;
    for (std::size_t i = 1; i != leafNodes.size(); i++)
    {
        actual = CountBlackNodes(leafNodes[i], axiomTree.GetRoot());
        EXPECT_EQ(actual, reference);
    }
}

/*** Konec souboru black_box_tests.cpp ***/
