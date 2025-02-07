<?php

/**
 * @file
 * A taxonomy specific coder based on path aliases of terms.
 *
 * This handler reuses the path aliases of terms and hence avoids term ids in
 * the facet URLs. It requires the default pattern for taxonomy terms
 * ([term:vocabulary]/[term:name]), which can be configured at
 * admin/config/search/path/patterns. The alias of [term:name] will be reused
 * and combined with the facet alias ([facet:alias]/[term:name]).
 */

/**
 * Taxonomy pathauto specific implementation of FacetApiPrettyPathsCoder.
 */
class FacetApiPrettyPathsCoderTaxonomyPathauto extends FacetApiPrettyPathsCoderDefault {

  /**
   * @inhericdoc
   *
   * Taxonomy pathauto special case: <facet alias>/<term-name alias>
   */
  public function encodePathSegment(array $args) {
    $voc_alias = $this->getVocabularyPathAlias($args['facet'], $args['adapter']);
    if ($voc_alias) {
      if ($term = taxonomy_term_load($args['segment']['value'])) {
        // Get the alias ([term:vocabulary]/[term:name]) for this term and
        // extract the term:name part.
        $alias = backdrop_lookup_path('alias', 'taxonomy/term/' . $term->tid);
        if ($alias) {
          $parts = explode('/', $alias);
          if (count($parts) == 2) {
            $args['segment']['value'] = $parts[1];
          }
        }
      }
    }
    return parent::encodePathSegment($args);
  }

  /**
   * @inheritdoc
   *
   * Taxonomy pathauto special case: <facet alias>/<term-name alias>
   */
  public function decodePathSegmentValue(array $args) {
    $voc_alias = $this->getVocabularyPathAlias($args['facet'], $args['adapter']);
    if ($voc_alias && $args['value'] != '!') {
      // Rebuild the term alias, get the source (taxonomy/term/[term:tid]) and
      // extract the term id.
      $source = backdrop_lookup_path('source', $voc_alias . '/' . $args['value']);
      if ($source) {
        $exploded = explode('/', $source);
        if (count($exploded) == 3) {
          $args['value'] = $exploded[2];
        }
      }
      else {
        // If term doesn't exist, just make the value 0. This can help avoid
        // errors down the line if searches are expecting integer values
        $args['value'] = 0;
      }
    }
    return parent::decodePathSegmentValue($args);
  }

  /**
   * Helper function that returns the path alias for a vocabulary.
   *
   * @param array $facet_info
   * @param FacetapiAdapter $adapter
   *
   * @return string|false
   */
  private function getVocabularyPathAlias($facet_info, FacetapiAdapter $adapter) {
    static $aliases = array();
    module_load_include('inc', 'path');
    if (!isset($aliases[$facet_info['name']])) {
      $aliases[$facet_info['name']] = FALSE;
      $facet_settings = $adapter->getFacetSettingsGlobal($facet_info);
      $voc = taxonomy_vocabulary_load($facet_settings->settings['pretty_paths_taxonomy_pathauto_vocabulary']);
      if ($voc) {
        $aliases[$facet_info['name']] = path_clean_string($voc->name);
      }
    }
    return $aliases[$facet_info['name']];
  }
}
