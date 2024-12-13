<?php

/**
 * @file
 * The default base path provider.
 */

/**
 * Default FacetApiPrettyPathsBasePathProvider.
 */
class FacetApiPrettyPathsDefaultBasePathProvider implements FacetApiPrettyPathsBasePathProvider {

  /**
   * @inheritdoc
   */
  public function getBasePath(FacetapiUrlProcessorPrettyPaths $urlProcessor) {
    return $urlProcessor->getPathWithoutSegments();
  }

}
