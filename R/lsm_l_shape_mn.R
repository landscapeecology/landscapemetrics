#'  Shape index  (class level)
#'
#' @description Perimeter divided by squareroot of area
#'
#' @param landscape Raster* Layer, Stack, Brick or a list of rasterLayers.
#'
#' @return tibble
#'
#' @examples
#' lsm_l_shape_mn(landscape)
#' lsm_l_shape_mn(landscape_stack)
#'
#' @aliases lsm_l_shape_mn
#' @rdname lsm_l_shape_mn
#'
#' @references
#' McGarigal, K., and B. J. Marks. 1995. FRAGSTATS: spatial pattern analysis
#' program for quantifying landscape structure. USDA For. Serv. Gen. Tech. Rep.
#'  PNW-351.
#'
#' @export
lsm_l_shape_mn <- function(landscape) UseMethod("lsm_l_shape_mn")

#' @name lsm_l_shape_mn
#' @export
lsm_l_shape_mn.RasterLayer <- function(landscape) {
    purrr::map_dfr(raster::as.list(landscape), lsm_l_shape_mn_calc, .id = "layer") %>%
        dplyr::mutate(layer = as.integer(layer))
}

#' @name lsm_l_shape_mn
#' @export
lsm_l_shape_mn.RasterStack <- function(landscape) {
    purrr::map_dfr(raster::as.list(landscape), lsm_l_shape_mn_calc, .id = "layer") %>%
        dplyr::mutate(layer = as.integer(layer))

}

#' @name lsm_l_shape_mn
#' @export
lsm_l_shape_mn.RasterBrick <- function(landscape) {
    purrr::map_dfr(raster::as.list(landscape), lsm_l_shape_mn_calc, .id = "layer") %>%
        dplyr::mutate(layer = as.integer(layer))

}

#' @name lsm_l_shape_mn
#' @export
lsm_l_shape_mn.list <- function(landscape) {
    purrr::map_dfr(landscape, lsm_l_shape_mn_calc, .id = "layer") %>%
        dplyr::mutate(layer = as.integer(layer))

}

lsm_l_shape_mn_calc <- function(landscape){

    shape_mn <- landscape %>%
        lsm_p_shape() %>%
        dplyr::summarise(value = mean(value))

    tibble::tibble(
        level = "patch",
        class = as.integer(NA),
        id = as.integer(NA),
        metric = "shape index (mean)",
        value = shape_mn$value
    )
}