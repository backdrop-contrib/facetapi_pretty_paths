<?php

/**
 * @file
 * Base path provider interface.
 */

/**
 * Default FacetApiPrettyPathsBasePathProvider.
 */
interface FacetApiPrettyPathsBasePathProvider {

  /**
   * @param FacetapiUrlProcessorPrettyPaths $urlProcessor
   * @return string
   *   base path.
   */
  public function getBasePath(FacetapiUrlProcessorPrettyPaths $urlProcessor);

}
