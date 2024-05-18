//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Adam Zvara <xzvara01@stud.fit.vutbr.cz>
// $Date:       $2021-02-21
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Adam Zvara
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//

class TestMatrix: public::testing::Test
{

    protected:
        Matrix basicMatrix()
        {
            return Matrix();
        }

        Matrix identity2x2()
        {
            Matrix matrix = Matrix(2,2);
            matrix.set(std::vector<std::vector<double>> 
            {
	        {1, 0},
		{0, 1},
            });
            return matrix;
        }

        Matrix identity3x3()
        {
            Matrix matrix = Matrix(3,3);
            matrix.set(std::vector<std::vector<double>> 
            {
	        {1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
            });
            return matrix;
        } 

        Matrix matrix2x3()
        {
            Matrix matrix = Matrix(2,3);
            matrix.set(std::vector<std::vector<double>> 
            {
	        {1, 1, 1},
		{2, 2, 2},
            });
            return matrix;
        }

        Matrix matrix4x2()
        {
            Matrix matrix = Matrix(4,2);
            matrix.set(std::vector<std::vector<double>> 
            {
                {2,8.1},
                {7,0.5},
                {1,15.8},
                {3,12.4},
            });
            return matrix;
        }
};

TEST_F(TestMatrix, Contructors)
{
    EXPECT_NO_THROW(basicMatrix());
    EXPECT_NO_THROW(identity2x2());
    EXPECT_NO_THROW(identity3x3());
    EXPECT_NO_THROW(matrix4x2());

    EXPECT_ANY_THROW(Matrix b(0,0));
}

TEST_F(TestMatrix, SetGetFunctions)
{
    //Testy s maticou 1x1
    Matrix matrix = basicMatrix();
    double value = 1;
    EXPECT_TRUE(matrix.set(0,0,value));
    EXPECT_EQ(matrix.get(0,0), value);
    EXPECT_FALSE(matrix.set(5,5,5));
    EXPECT_ANY_THROW(matrix.get(5,5));
    
    //Testy s maticou 4x2
    matrix = matrix4x2();
    value = 0;
    EXPECT_TRUE(matrix.set(0,0,value));
    EXPECT_EQ(matrix.get(0,0), value);
    EXPECT_FALSE(matrix.set(4,3,value));
    EXPECT_ANY_THROW(matrix.get(4,4));

    //Nastavenie vacsej matice ako je povodna
    matrix = Matrix(1,2);
    EXPECT_FALSE(matrix.set(std::vector<std::vector<double>> 
    {
        {2,8,3},
        {3,12,0},
        {0,0,0},
    }));
}

TEST_F(TestMatrix, OperationEqual)
{
    Matrix a = basicMatrix(),
           b = basicMatrix(),
           c = identity3x3(),
           d = Matrix(3,3);

    EXPECT_TRUE(a==b);
    EXPECT_FALSE(c==d);
    EXPECT_ANY_THROW(a==c);
}

TEST_F(TestMatrix, OperationPlus)
{
    Matrix a = basicMatrix(),
           b = identity3x3(),
           c = Matrix(3,3),
           d = identity2x2(),
           d_result = Matrix(2,2);

    d_result.set(std::vector<std::vector <double>>
    {
        {2,0}, 
        {0,2},
    });

    EXPECT_EQ(a+a, a);
    EXPECT_EQ(b+c, b);
    EXPECT_EQ(d+d, d_result);
    EXPECT_ANY_THROW(a+c);
}

TEST_F(TestMatrix, OperationTimes)
{
    Matrix a = basicMatrix(),
           b = identity3x3(),
           c = matrix2x3(),
           d = matrix4x2();
    
    c.set((std::vector<std::vector<double>> 
    {
        {8,15},
        {20,9},
    }));

    Matrix result = Matrix(4,3);
    result.set((std::vector<std::vector<double>> 
    {
        {18.2,18.2,18.2},
        {8,8,8},
        {32.6,32.6,32.6},
        {27.8,27.8,27.8},
    }));

    EXPECT_EQ(a*a, a);
    EXPECT_EQ(c*b, c);
    EXPECT_EQ(d*c, result);
    EXPECT_ANY_THROW(a*d);
    EXPECT_ANY_THROW(b*d);


    result = Matrix(3,3);
    result.set((std::vector<std::vector<double>> 
    {
        {3,0,0},
        {0,3,0},
        {0,0,3},
    }));

    EXPECT_EQ(a*1, a);
    EXPECT_EQ(b*3, result);
}

TEST_F(TestMatrix, Equation)
{
    Matrix a = basicMatrix(),
           b = identity3x3(),
           c = matrix2x3(),
           d = Matrix(2,2),
           e = Matrix(4,4);


    a.set(0,0,1);
    d.set((std::vector<std::vector<double>> 
    {
        {1,2},
        {4,2},
    }));
    
    e.set(std::vector<std::vector<double>>
    {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1},
    });


    EXPECT_EQ(a.solveEquation(std::vector<double> {2}), (std::vector<double> {2}));
    EXPECT_EQ(d.solveEquation(std::vector<double> {6,6}), (std::vector<double> {0,3}));
    EXPECT_EQ(b.solveEquation(std::vector<double> {8,2,4}), (std::vector<double> {8,2,4}));
    EXPECT_EQ(e.solveEquation(std::vector<double> {1,1,1,1}), (std::vector<double> {1,1,1,1}));
    EXPECT_ANY_THROW(c.solveEquation(std::vector<double> {1,2}));
    EXPECT_ANY_THROW(c.solveEquation(std::vector<double> {1,2,3}));
    

    b.set(2,2,0);
    EXPECT_ANY_THROW(b.solveEquation(std::vector<double>{0,0,0}));
}

TEST_F(TestMatrix, Transposed)
{
    Matrix a = basicMatrix(),
           b = identity3x3(),
           c = matrix2x3(),
           d = Matrix(2,2),
           c_result = Matrix(3,2),
           d_result = Matrix(2,2);

    d.set(std::vector<std::vector<double>>
    {
        {8,2},
        {7,5},
    });

    d_result.set(std::vector<std::vector<double>> 
    {
        {8,7},
        {2,5},
    });

    c_result.set(std::vector<std::vector<double>> 
    {
        {1,2},
        {1,2},
        {1,2},
    });

    EXPECT_EQ(a.transpose(), a);
    EXPECT_EQ(b.transpose(), b);
    EXPECT_EQ(c.transpose(), c_result);
    EXPECT_EQ(d.transpose(), d_result);
}

TEST_F(TestMatrix, Inversed){
    Matrix a = basicMatrix(),
           b = identity3x3(),
           c = matrix4x2(),
           d = Matrix(2,2),
           e = identity2x2(),
           f = Matrix(3,3),
           f_result = Matrix(3,3);

    f.set(std::vector<std::vector<double>>
    {
        {1,0,-1},
        {-1,1,0},
        {1,-2,2},
    });

    d.set(std::vector<std::vector<double>>
    {
        {0,-1},
        {1,2},
    });

    EXPECT_EQ(b.inverse(), b);
    EXPECT_EQ((d*d.inverse()), e);
    EXPECT_EQ((f*f.inverse()), b);
    EXPECT_ANY_THROW(a.inverse());
    EXPECT_ANY_THROW(c.inverse());

    d.set(std::vector<std::vector<double>>
    {
        {0,0},
        {0,0},
    });

    b.set(2,2,0);
    EXPECT_ANY_THROW(b.inverse());
    EXPECT_ANY_THROW(d.inverse());
}
/*** Konec souboru white_box_tests.cpp ***/
