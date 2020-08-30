context("landscape_as_list")

test_that("landscape_as_list returns a list", {
    expect_is(landscapemetrics:::landscape_as_list(landscape), "list")
    expect_is(landscapemetrics:::landscape_as_list(landscape_stack), "list")
    expect_is(landscapemetrics:::landscape_as_list(landscape_brick), "list")
    expect_is(landscapemetrics:::landscape_as_list(landscape_list), "list")
    })


# additional test for stars and terra
# landscape_stars = stars::st_as_stars(landscape)
# landscape_terra = terra::rast(landscape)
# expect_is(landscapemetrics:::landscape_as_list(landscape_stars), "list")
# expect_is(landscapemetrics:::landscape_as_list(landscape_terra), "list")