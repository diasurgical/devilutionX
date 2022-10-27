# Program to print the fibonacci series upto n_terms

# Recursive function
def recursive_fibonacci(n):
if n <= 1:
	return n
else:
	return(recursive_fibonacci(n-1) + recursive_fibonacci(n-2))

n_terms = 10

# check if the number of terms is valid
if n_terms <= 0:
print("Invalid input ! Please input a positive value")
else:
print("Fibonacci series:")
for i in range(n_terms):
	print(recursive_fibonacci(i))
