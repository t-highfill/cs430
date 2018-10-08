
#ifndef __LINEAR_H__
#define __LINEAR_H__

#include <cstdio>
#include <algorithm>

template<class T, size_t R, size_t C>
class Matrix{
private:
	bool _delete;
	T **_mat;
	Matrix(T** matrix){
		_delete = false;
		_mat = matrix;
	}
	T det(size_t i, int* colSet) const{
		size_t N = R - i;
		if(N == 1){
			for(size_t j=0; j<C; j++){
				if(colSet[j] == 0){
					return _mat[i][j];
				}
			}
			throw std::runtime_error("Dimension mismatch!");
		}
		T res = 0;
		T sgn = 1;
		for(size_t j=0; j<C; j++){
			if(colSet[j] == 0){
				colSet[j] = 1;
				res += sgn*_mat[i][j]*det(i+1, colSet);
				colSet[j] = 0;
				sgn = -sgn;
			}
		}
		return res;
	}
public:
	Matrix(){
		_delete = true;
		_mat = new T*[R];
		for(size_t i=0; i<R; i++){
			_mat[i] = new T[C];
		}
	}
	Matrix(const Matrix<T, R, C>& copy) : Matrix(){
		generate([copy](size_t i, size_t j){return copy._mat[i][j];});
	}
	Matrix(Matrix<T, R, C>&& move){
		_delete = move._delete;
		_mat = move._mat;
		move._mat = nullptr;
	}
	~Matrix(){
		if(_delete && _mat != nullptr){
			for(size_t i; i<R; i++){
				delete[] _mat[i];
			}
			delete[] _mat;
		}
	}
	friend void swap(Matrix<T, R, C>& first, Matrix<T, R, C>& second){
		using std::swap;
		std::swap(first._delete, second._delete);
		std::swap(first._mat, second._mat);
	}
	Matrix<T, R, C>& operator=(Matrix<T, R, C> other){
		swap(*this, other);
		return *this;
	}
	Matrix<T, R, C> copy() const{
		Matrix<T, R, C> res(*this);
		return res;
	}
	bool operator==(const Matrix<T, R, C> other) const{
		for(size_t i=0; i<R; i++){
			T *row1 = _mat[i];
			T *row2 = other._mat[i];
			for(size_t j=0; j<C; j++){
				if(row1[j] != row2[j]){
					return false;
				}
			}
		}
		return true;
	}
	bool operator!=(const Matrix<T, R, C> other) const{
		return !((*this) == other);
	}
	T& get(size_t i, size_t j){
		if(i >= R || j >= C){
			throw std::runtime_error("Index out of bounds");
		}
		return _mat[i][j];
	}
	const T& get(size_t i, size_t j) const{
		return _mat[i][j];
	}
	Matrix<T, R, C>& generate(auto gen){
		for(size_t i=0; i<R; i++){
			T *row = _mat[i];
			for(size_t j=0; j<C; j++){
				row[j] = gen(i,j);
			}
		}
		return *this;
	}
	Matrix<T, R, C>& map(auto f){
		for(size_t i=0; i<R; i++){
			T *row = _mat[i];
			for(size_t j=0; j<C; j++){
				row[j] = f(row[j]);
			}
		}
		return *this;
	}
	Matrix<T, R, C>& locMap(auto f){
		for(size_t i=0; i<R; i++){
			T *row = _mat[i];
			for(size_t j=0; j<C; j++){
				row[j] = f(i,j,row[j]);
			}
		}
		return *this;
	}
	Matrix<T, R, C>& multiply(const T scalar){
		return map([scalar](T val){return val*scalar;});
	}
	template<size_t R2>
	Matrix<T, R, R2>& multiply(const Matrix<T, C, R2>& other){
		Matrix<T, R, R2> res;
		return res.generate([this, other](size_t i, size_t j){
			T sum = 0;
			T* row = _mat[i];
			for(size_t k=0; k<C; k++){
				sum += row[k]*other._mat[k][j];
			}
			return sum;
		});
	}
	Matrix<T, R, C>& neg(){
		return map([](T val){return -val;});
	}
	Matrix<T, R, C>& divide(const T scalar){
		return multiply(1/scalar);
	}
	Matrix<T, R, C>& add(const T scalar){
		return map([scalar](T val){return val+scalar;});
	}
	Matrix<T, R, C>& add(const Matrix<T, R, C>& other){
		return locMap([other](size_t i, size_t j, T val){return val+other._mat[i][j];});
	}
	Matrix<T, R, C>& sub(const T scalar){
		return add(-scalar);
	}
	Matrix<T, R, C>& sub(const Matrix<T, R, C>& other){
		return locMap([other](size_t i, size_t j, T val){return val-other._mat[i][j];});
	}
	Matrix<T, R, C>& fill(const T val){
		return generate([val](size_t i, size_t j){return val;});
	}
	Matrix<T, 1, C> row(size_t idx){
		T *row = _mat[idx];
		Matrix<T, 1, C> res(&row);
		return res;
	}
	Matrix<T, R, 1> column(size_t idx){
		T **col = new T*[R];
		for(size_t i=0;i<R;i++){
			col[i] = _mat[i]+idx;
		}
		Matrix<T, R, 1> res(col);
		return res;
	}
	T determinant() const{
		if(R != C){
			throw std::runtime_error("Determinant is only defined for square matrices");
		}
		int mask[C] = {0};
		return det(0, mask);
	}
	bool isInvertible() const{
		return R == C && determinant() != 0;
	}
	Matrix<T, R, C> inverse() const{
		// Ensure that the matrix is square
		if(R != C){
			throw std::runtime_error("Inverse is only defined for square matrices");
		}
		// Use Gauss-Jordan elimination to find the inverse of this matrix
		const size_t N = R;
		Matrix<T, N, N> id;
		id.generate([](size_t i, size_t j){return (i==j) ? 1 : 0;});
		// These two matrices make up the two halves of our "bar matrix".
		// The left is a copy of our matrix and the right is the identity matrix.
		// We'll apply elementary row operations to transform the left side into the identity
		// matrix. At that point the right side will have become our inverse matrix
		Matrix<T, N, N> left(*this), right(id);
		// Look for and try to remove zeros on the main diagonal
		for(size_t j=0; j<N; j++){
			T div = left.get(j,j);
			// if the value is 0 we need to add a row that doesn't have a zero in this column
			for(size_t i=0; div == 0 && i<N; i++){
				T tmp = left.get(i,j);
				if(tmp != 0){
					left.row(j).add(left.row(i));
					right.row(j).add(right.row(i));
					div = tmp;
				}
			}
			if(div == 0){
				// If we can't find a nonzero in this column the matrix is not invertible
				throw std::runtime_error("Matrix is not invertible");
			}
		}
		// Use elemntary row operations to transform the two matrices
		for(size_t j=0; left!=id && j<N; j++){
			T div = left.get(j,j);
			// Divide the row by the value on the diagonal. That value will become 1
			Matrix<T, 1, N> rowL = left.row(j).divide(div);
			Matrix<T, 1, N> rowR = right.row(j).divide(div);
			// Make the other values in this column 0 by subtracting multiples of this row
			for(size_t i=0; i<N; i++){
				if(i != j){
					T mul = left.get(i,j);
					left.row(i).sub(rowL.copy().multiply(mul));
					right.row(i).sub(rowR.copy().multiply(mul));
				}
			}
		}
		if(left != id){
			// Something went wrong
			throw std::runtime_error("Failed to transform left side matrix");
		}
		return right;
	}
};

template<class T, size_t R, size_t C>
Matrix<T, R, C> operator*(Matrix<T, R, C> mat, const T scalar){
	return mat.multiply(scalar);
}

template<class T, size_t R, size_t C>
Matrix<T, R, C> operator*(const T scalar, Matrix<T, R, C> mat){
	return mat.multiply(scalar);
}

template<class T, size_t R, size_t C, size_t R2>
Matrix<T, R, R2> operator*(const Matrix<T, R, C> mat1, const Matrix<T, C, R2> mat2){
	return mat1.multiply(mat2);
}

template<class T, size_t R, size_t C>
Matrix<T, R, C> operator+(Matrix<T, R, C> mat, const T scalar){
	Matrix<T, R, C> res(mat);
	return res.add(scalar);
}

template<class T, size_t R, size_t C>
Matrix<T, R, C> operator+(const T scalar, Matrix<T, R, C> mat){
	Matrix<T, R, C> res(mat);
	return res.add(scalar);
}

template<class T, size_t R, size_t C>
Matrix<T, R, C> operator+(Matrix<T, R, C> mat1, const Matrix<T, R, C> mat2){
	return mat1.add(mat2);
}

#endif