<?php

/**
 * @file
 * A taxonomy specific coder for pretty paths.
 */

/**
 * Taxonomy specific implementation of FacetApiPrettyPathsCoder.
 */
class FacetApiPrettyPathsCoderTaxonomy extends FacetApiPrettyPathsCoderDefault {

  /**
   * @inheritdoc
   *
   * Taxonomy special case: <alias>/<term-name>-<term-id>
   */
  public function encodePathSegment(array $args) {
    if ($term = taxonomy_term_load($args['segment']['value'])) {
      $args['segment']['value'] = $this->prettyPath($term->name) . '-' . $term->tid;
    }
    return parent::encodePathSegment($args);
  }

  /**
   * @inheritdoc
   *
   * Taxonomy special case: <alias>/<term-name>-<term-id>
   */
  public function decodePathSegmentValue(array $args) {
    $exploded = explode('-', $args['value']);
    $args['value'] = array_pop($exploded);
    return parent::decodePathSegmentValue($args);
  }
}
