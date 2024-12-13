<?php

/**
 * @file
 * A Facet API Adapter based base path provider.
 */

/**
 * Facet API Adapter based FacetApiPrettyPathsBasePathProvider.
 */
class FacetApiPrettyPathsAdapterBasePathProvider implements FacetApiPrettyPathsBasePathProvider {

  /**
   * @inheritdoc
   */
  public function getBasePath(FacetapiUrlProcessorPrettyPaths $urlProcessor) {
    return $urlProcessor->getAdapter()->getSearchPath();
  }

}
