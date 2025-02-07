<?php

/**
 * @file
 * A default coder for pretty paths.
 */

/**
 * Default FacetApiPrettyPathsCoder.
 */
class FacetApiPrettyPathsCoderDefault {

  /**
   * Encode a path segment in order to output it as part of a pretty path.
   *
   * @param array $args
   *   An associative array containing:
   *   - segment: The path segment to encode.
   *   - facet: The facet that triggered the segment.
   * @return array
   */
  public function encodePathSegment(array $args) {
    // Default: <alias>/<value>
    $args['segment']['value'] = str_replace('/', '%2F', $args['segment']['value']);
    $args['segment']['value'] = str_replace('.', '%2E', $args['segment']['value']);
    $args['segment']['alias'] = rawurlencode($args['segment']['alias']);

    return $args;
  }

  /**
   * Decode the alias part of a path segment in order
   * to interpret it from a given pretty path.
   *
   * @param array $args
   *   An associative array containing:
   *   - alias: The alias to decode.
   * @return string
   *   Decoded alias
   */
  public function decodePathSegmentAlias(array $args) {
    $alias = $args['alias'];
    // We need to decode path aliases twice.
    $alias = rawurldecode($alias);
    $alias = rawurldecode($alias);
    return $alias;
  }

  /**
   * Decode the value part of a path segment in order
   * to interpret it from a given pretty path.
   *
   * @param array $args
   *   An associative array containing:
   *   - segment: The path segment to decode.
   *   - facet: The facet that is related to the path segment.
   * @return string
   *   Decoded path value
   */
  public function decodePathSegmentValue(array $args) {
    $value = $args['value'];
    $value = str_replace('%2F', '/', rawurldecode($value));
    $value = str_replace('%2E', '.', rawurldecode($value));
    return $value;
  }

  /**
   * Convert a given text to a pretty path using pathauto, if available.
   *
   * @param string $text
   * @return string
   */
  protected function prettyPath($text) {
    // @todo: Make this pluggable?
    module_load_include('inc', 'path');
    return path_clean_string($text);
  }
}
