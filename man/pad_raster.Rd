% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/pad_raster.R
\name{pad_raster}
\alias{pad_raster}
\title{pad_raster}
\usage{
pad_raster(
  landscape,
  pad_raster_value = -999,
  pad_raster_cells = 1,
  global = FALSE,
  return_raster = TRUE,
  to_disk = getOption("to_disk", default = FALSE)
)
}
\arguments{
\item{landscape}{Raster* Layer, Stack, Brick, SpatRaster (terra), stars, or a list of rasterLayers.}

\item{pad_raster_value}{Value of cells added}

\item{pad_raster_cells}{Number of rows and columns added}

\item{global}{Only pad around the raster extent or also NA holes "inside"}

\item{return_raster}{If false, matrix is returned}

\item{to_disk}{Logical argument, if FALSE results of get_patches are hold
in memory. If true, pad_raster writes temporary files and hence, does not hold
everything in memory. Can be set with a global option, e.g. \code{option(to_disk = TRUE)}.}
}
\value{
raster
}
\description{
Adding padding to raster
}
\details{
Adds equally (in all four directions) additional cells around the raster
}
\examples{
pad_raster(landscape, pad_raster_value = -1, pad_raster_cells = 2)

}
\keyword{internal}
