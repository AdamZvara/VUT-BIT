# author: Adam Zvara (xzvara01)
# IAM Logika 1

import sys
import numpy as np

# Parse arguments
def print_usage():
    print("Calculate CNF of generalized eight-queens problem\nUsage: python queens.py NUMBER")
    exit(1)
try:
    N = int(sys.argv[1])
    if (N <= 0):
        raise Exception()
except Exception:
    print_usage()

# Function to calculate sum of range, e.g fn(5) = 4+3+2+1
def fn(x):
    return sum(range(x))

# Create rows and columns as 2D array
row_vars = np.arange(1, N*N+1).reshape(N, N)
col_vars = row_vars.T

# Get all of array diagonals: (source) https://stackoverflow.com/a/6313414
diags_one = [row_vars[::-1,:].diagonal(i) for i in range(-row_vars.shape[0]+1, row_vars.shape[1])][1:-1]
diags_two = [row_vars.diagonal(i) for i in range(row_vars.shape[1]-1, -row_vars.shape[0], -1)][1:-1]

x = map(fn, [len(i) for i in diags_one]) # Number of pairs on each diagonal
# Print "problem line", number of variables is N*N
# Number of clauses is:
#   fn for each row/column (number of pairs) +
#   each row/column first posibility (e.g for 1st row first clause is 1 and 2 and 3 and ...) +
#   fn for each diagonal (number of pairs)
print(f"p cnf {N*N} {fn(N) * 2*N + 2*N + sum(list(x)) * 2}")

# Print list of elements in array in DIMACS CNF form
def print_arr(arr):
    for i in arr:
        print(i, end=" ")
    print("0")

# Print negative clauses for given array in DIMACS CNF form
# e.g negative_pair_clause([1,2,3]) -> -1 -2 0, -1 -3 0, -2 -3 0
def negative_pair_clausule(arr):
    for i in range(len(arr)-1):
        for j in range(i+1, len(arr)):
            print(-arr[i], -arr[j], end=" ")
            print("0")

# Print rules for single row on colum given in parameter
def row_column_rules(arr):
    for i in arr:
        print_arr(i) # First clause containing all options
        negative_pair_clausule(i) # Negative clauses for pairs

# Make sure there is only one queen per row
row_column_rules(row_vars)
# Make sure there is only one queen per column
row_column_rules(col_vars)

# Make sure there is only one queen per diagonal
for i in diags_two:
    negative_pair_clausule(i)
for i in diags_one:
    negative_pair_clausule(i)