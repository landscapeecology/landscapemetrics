#include <RcppArmadillo.h>
using namespace Rcpp;

// [[Rcpp::export]]
IntegerMatrix rcpp_xy_from_matrix2(arma::imat x, Rcpp::Nullable<Rcpp::IntegerVector> cell = R_NilValue) {
    // adapted from raster::xyFromCell()
    // get number of rows and columns
    int n_rows = x.n_rows;
    int n_cols = x.n_cols;
    // init objects
    size_t len;
    IntegerVector cells;

    if (cell.isNotNull()){
        // calculate only for selected cells
        // create a vector with cells ids
        cells = IntegerVector(cell);
        // calculate a number of cells
        len = cells.size();
    } else {
        // calculate the whole matrix
        // calculate a number of cells
        len = n_rows * n_cols;
        // create a vector with cells ids
        cells = seq(1, len) - 1;
    }
    // create a template two column matrix for a result
    IntegerMatrix result(len, 2);
    // for each cell...
    for (size_t i = 0; i < len; i++) {
        // ...get cell id
        int c = cells[i];
        // ...get column number
        size_t col = (c / n_rows);
        // ...get row number
        size_t row = std::fmod(c, n_rows);
        // ...insert cols and rows
        result(i, 1) = col;
        result(i, 0) = row;
    }
    return result;
}

// [[Rcpp::export]]
IntegerVector rcpp_cell_from_xy2(arma::imat x, IntegerMatrix y) {
    // adapted from raster::cellFromXY()
    // get number of rows and columns
    int n_rows = x.n_rows;
    int n_cols = x.n_cols;
    // get length of the query
    size_t len = y.nrow();
    IntegerVector result(len);
    // for each query...
    for (size_t i = 0; i < len; i++) {
        // extract column and row of the query
        double col = y(i, 1);
        double row = y(i, 0);
        // calculate cell numbers (NA if outside of the input matrix)
        if (row < 0 || row >= n_rows || col < 0 || col >= n_cols) {
            result[i] = NA_INTEGER;
        } else {
            result[i] = col * n_rows + row;
        }
    }
    return result;
}

// [[Rcpp::export]]
IntegerMatrix rcpp_create_neighborhood2(arma::imat directions){
    if (directions.n_elem == 1){
        int x = directions(0);
        IntegerVector x_id(x);
        IntegerVector y_id(x);
        if (x == 4){
            x_id = IntegerVector::create(0, -1, 1, 0);
            y_id = IntegerVector::create(-1, 0, 0, 1);
        } else if (x == 8){
            x_id = IntegerVector::create(-1, 0, 1, -1, 1, -1, 0, 1);
            y_id = IntegerVector::create(-1, -1, -1, 0, 0, 1, 1, 1);
        }
        IntegerMatrix neigh_coords(x_id.size(), 2);
        neigh_coords(_, 0) = x_id;
        neigh_coords(_, 1) = y_id;
        return neigh_coords;
    } else {
        IntegerVector center_position = as<IntegerVector>(wrap(find(directions == 0)));
        IntegerMatrix center_coords = rcpp_xy_from_matrix2(directions, center_position);

        IntegerVector neigh_position = as<IntegerVector>(wrap(find(directions == 1)));
        IntegerMatrix neigh_coords = rcpp_xy_from_matrix2(directions, neigh_position);

        neigh_coords(_,0) = neigh_coords(_,0) - center_coords(0, 0);
        neigh_coords(_,1) = neigh_coords(_,1) - center_coords(0, 1);

        return neigh_coords;
    }
}

// [[Rcpp::export]]
IntegerMatrix rcpp_get_adjacency(arma::imat x, arma::imat directions) {
    // extract coordinates from matrix
    IntegerMatrix xy = rcpp_xy_from_matrix2(x);

    // get a number of rows
    int xy_nrows = xy.nrow();
    // create neighbots coordinates
    IntegerMatrix neigh_coords = rcpp_create_neighborhood2(directions);
    int neigh_len = neigh_coords.nrow();
    // repeat neighbots coordinates
    IntegerMatrix neigh_coords_rep(neigh_len * xy_nrows, 2);
    neigh_coords_rep(_, 0) = rep_each(neigh_coords(_, 0), xy_nrows);
    neigh_coords_rep(_, 1) = rep_each(neigh_coords(_, 1), xy_nrows);

    // repreat center cells coordinates
    IntegerMatrix neighs(neigh_len * xy_nrows, 2);
    neighs(_, 0) = rep(as<IntegerVector>(wrap(xy(_, 0))), xy_nrows);
    neighs(_, 1) = rep(as<IntegerVector>(wrap(xy(_, 1))), xy_nrows);

    // move coordinates (aka get neighbors)
    neighs(_, 0) = neighs(_, 0) + neigh_coords_rep(_, 0);
    neighs(_, 1) = neighs(_, 1) + neigh_coords_rep(_, 1);

    // extract center cells cell numbers
    IntegerVector center_cells_unrep = rcpp_cell_from_xy2(x, xy);
    // repeat center cells cell numbers
    IntegerVector center_cells(neigh_len * xy_nrows);
    center_cells = rep(center_cells_unrep, neigh_len);
    // extract neighbors cell numbers
    IntegerVector neighs_cells = rcpp_cell_from_xy2(x, neighs);
    // combine the results
    IntegerMatrix result(center_cells.size(), 2);
    result(_, 0) = center_cells;
    result(_, 1) = neighs_cells;
    return result;
}

// [[Rcpp::export]]
IntegerMatrix rcpp_get_pairs(arma::imat x, arma::imat directions) {
    // extract adjency pairs
    IntegerMatrix adjency_pairs = rcpp_get_adjacency(x, directions);
    // number of pairs
    int num_pairs = adjency_pairs.nrow();
    // result template matrix
    IntegerMatrix result(num_pairs, 2);
    // for each pair...
    for (int i = 0; i < num_pairs; i++) {
        // get neighbor and central cell
        int neigh_cell = adjency_pairs(i, 1);
        int center_cell = adjency_pairs(i, 0);
        // if neighbor exist then input values
        if (neigh_cell != INT_MIN){
            result(i, 0) = x(center_cell);
            result(i, 1) = x(neigh_cell);
        } else {
            result(i, 0) = x(center_cell);
            result(i, 1) = NA_INTEGER;
        }
    }
    return result;
}


// [[Rcpp::export]]
IntegerMatrix rcpp_get_coocurrence_matrix_old(arma::imat x, arma::imat directions) {
    // get unique values
    arma::ivec u = arma::conv_to<arma::ivec>::from(arma::unique(x.elem(find(x != INT_MIN))));
    // create a matrix of zeros of unique values size
    arma::imat cooc_mat(u.n_elem, u.n_elem, arma::fill::zeros);
    // extract adjency pairs
    IntegerMatrix pairs = rcpp_get_pairs(x, directions);
    // number of pairs
    int num_pairs = pairs.nrow();
    // for each row and col
    for (int i = 0; i < num_pairs; i++) {
        // extract value of central cell and its neighbot
        int center = pairs(i, 0);
        int neigh = pairs(i, 1);
        // find their location in the output matrix
        arma::uvec loc_c = find(u == center);
        arma::uvec loc_n = find(u == neigh);
        // add its count
        cooc_mat(loc_c, loc_n) += 1;
    }
    // return a coocurence matrix
    IntegerMatrix cooc_mat_result = as<IntegerMatrix>(wrap(cooc_mat));
    // add names
    List u_names = List::create(u, u);
    cooc_mat_result.attr("dimnames") = u_names;
    return cooc_mat_result;
}

/*** R
library(raster)
library(dplyr)
test <- landscapemetrics::augusta_nlcd
# test <- raster("~/Desktop/lc_2008_4bit_clip.tif") # produces a matrix filled with NA ????

mat <- raster::as.matrix(test)
four <- as.matrix(4)

bench::mark(
    new = rcpp_get_coocurrence_matrix(mat, four),
    old = rcpp_get_coocurrence_matrix_old(mat, four),
    iterations = 500,
    check = TRUE,
    relative = TRUE
)
*/