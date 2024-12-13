<?php

/**
 * @file
 * A custom URL processor for pretty paths.
 */

/**
 * Extension of FacetapiUrlProcessor.
 */
class FacetapiUrlProcessorPrettyPaths extends FacetapiUrlProcessorStandard {

  /**
   * An array of filter params.
   *
   * @var array
   */
  protected $filterParams = array();

  /**
   * An array of pretty path segments.
   *
   * @var array
   */
  protected $pathSegments = array();

  /**
   * The adapter that uses this FacetapiUrlProcessor.
   *
   * @var FacetapiAdapter
   */
  protected $adapter = array();

  /**
   * Options that apply for this FacetapiUrlProcessor.
   *
   * @var array
   */
  protected $options = array();

  /**
   * The full path string, as we override $_GET['q].
   *
   * @var string|null
   */
  protected $fullPath = NULL;

  /**
   * The base path for constructing pretty facet paths.
   *
   * @var string|null
   */
  protected $pathWithoutSegments = NULL;

  /**
   * The base path for constructing pretty facet paths.
   *
   * @var FacetApiPrettyPathsBasePathProvider
   */
  protected $basePathProvider;

  /**
   * @inheritdoc
   */
  function __construct(FacetapiAdapter $adapter) {
    $config = config('facetapi_pretty_paths.settings');
    $this->adapter = $adapter;
    $this->options = $config->get('searcher.' . $this->adapter->getSearcher() . '.options');
    $base_path_provider = isset($this->options['base_path_provider']) ? $this->options['base_path_provider'] : 'default';
    $class = plugin_manager_load_class('facetapi_pretty_paths', 'base_path_provider', $base_path_provider, 'handler');
    $this->basePathProvider = new $class;
  }

  /**
   * @inheritdoc
   */
  public function fetchParams() {

    // Skip pretty paths logic for admin pages, as
    // for pretty paths, we have to manipulate $_GET['q].
    // @todo: Fix this workaround?
    if (strpos($_GET['q'], 'admin/') === 0) {
      return $_GET;
    }

    $this->fullPath = $_GET['q'];
    $this->filterParams = array();
    $this->pathSegments = array();
    $facets = $this->adapter->getEnabledFacets();

    $args = explode('/', $_GET['q']);

    // Traverse all segments "<alias>/<value>" from right to left (array_pop).
    while (($value = array_pop($args)) !== NULL) {
      if (!($alias = array_pop($args))) {
        // Restore last url part, if this was an incomplete segment.
        array_push($args, $value);
        break;
      }
      $found = FALSE;
      $alias = $this->decodePathSegmentAlias($alias);

      // Look for matching facet.
      foreach ($facets as $facet_alias => $facet) {
        $pretty_paths_alias = $this->getFacetPrettyPathsAlias($facet);

        // Add to params if alias from url matches alias from facet settings.
        if ($pretty_paths_alias == $alias) {
          $found = TRUE;
          $value = $this->decodePathSegmentValue($value, $facet);

          $this->filterParams[] = rawurlencode($facet['field']) . ':' . $value;

          // "Copy" to prevent $segment['facet'] from being overridden.
          $my_facet = $facet;
          $segment = $this->getPathSegment($my_facet, $value);
          $this->pathSegments[$segment['key']] = $segment;
        }
      }
      // When no more facet path segments have been found, we assume the rest of the url as the
      // path without segments which might be used as base path.
      if (!$found) {
        array_push($args, $alias, $value);
        break;
      }
    }

    // Set the rest of the url as the search path without segments.
    $this->pathWithoutSegments = implode('/', $args);

    // Since we traversed all segments from right to left we need to reverse
    // them here. Reason is because the order matters for some facets.
    $this->pathSegments = array_reverse($this->pathSegments);
    $this->filterParams = array_reverse($this->filterParams);

    // Return params as expected.
    $params = $_GET;
    $params['q'] = $this->pathWithoutSegments;
    $params[$this->filterKey] = $this->filterParams;
    return $params;
  }

  /**
   * @inheritdoc
   *
   *  Remove the filter query part from params and return it.
   */
  public function getQueryString(array $facet, array $values, $active) {
    $params = $this->params;
    unset($params[$this->filterKey]);
    return $params;
  }

  /**
   * @inheritdoc
   *
   *  Pretty paths will be generated as "search/url/segment1/segment2/".
   *
   *  By default, a segment will look like:
   *    "<alias>/<value>".
   */
  public function getFacetPath(array $facet, array $values, $active) {
    $segments = $this->pathSegments;
    $active_items = $this->adapter->getActiveItems($facet);
    $settings = $this->adapter->getFacetSettingsGlobal($facet);

    // Adds to segments if inactive, removes if active.
    foreach ($values as $value) {
      $segment = $this->getPathSegment($facet, $value);
      if ($active && isset($active_items[$value])) {
        unset($segments[$segment['key']]);
      }
      elseif (!$active) {
        $segments[$segment['key']] = $segment;
      }
    }

    // If Facet API supports limiting active items, remove path segments. See http://drupal.org/node/1393928
    if (count($active_items) && isset($settings->settings['limit_active_items']) && $settings->settings['limit_active_items']) {
      foreach ($active_items as $active_item) {
        $segment = $this->getPathSegment($facet, $active_item['value']);
        unset($segments[$segment['key']]);
      }
    }

    $path = $this->constructPath($segments);
    return $path;
  }

  /* ### Generic helpers ### */

  /**
   * Construct a path from for a given array of filter segments.
   *
   * @param array $segments
   * @return string
   */
  public function constructPath(array $segments) {
    if (!empty($this->options['sort_path_segments'])) {
      // Sort to avoid multiple urls with duplicate content.
      uksort($segments, 'strnatcmp');
    }
    $path = $this->getBasePath();
    // Add all path segments.
    foreach ($segments as $key => $segment) {
      $this->encodePathSegment($segment, $segment['facet']);
      $path .=  '/' . $segment['alias'] . '/' . $segment['value'];
    }
    return $path;
  }

  /**
   * Generate a path segment for a given facet + value.
   *
   * @param array $facet
   *   Passed by reference.
   * @param string $value
   */
  protected function getPathSegment(&$facet, $value) {
    $pretty_paths_alias = $this->getFacetPrettyPathsAlias($facet);
    return array(
      'alias' => $pretty_paths_alias,
      'value' => $value,
      'facet' => $facet,
      'key' => $pretty_paths_alias . "_" . $value,
    );
  }

  /**
   * Encode path segment
   *
   * @param array $segment
   *   Passed by reference
   * @param array $facet
   */
  protected function encodePathSegment(array &$segment, array $facet) {
    facetapi_pretty_paths_coder_callback('encodePathSegment',
      array(
        'segment' => &$segment,
        'facet' => $facet,
        'adapter' => $this->adapter,
      )
    );
  }

  /**
   * Decode path segment alias
   *
   * @param string $alias
   * @return array
   */
  protected function decodePathSegmentAlias($alias) {
    return facetapi_pretty_paths_coder_callback('decodePathSegmentAlias',
      array(
        'alias' => $alias
      )
    );
  }

  /**
   * Decode path segment value
   *
   * @param string $value
   * @param array $facet
   * @return array
   */
  protected function decodePathSegmentValue($value, array $facet) {
    return facetapi_pretty_paths_coder_callback('decodePathSegmentValue',
      array(
        'facet' => $facet,
        'adapter' => $this->adapter,
        'value' => $value,
      )
    );
  }

  /**
   * Returns the pretty_paths_alias for a given facet.
   *
   * If there is no custom pretty_paths_alias settings, in will default
   * to rawurlencode($facet['field alias']).
   *
   * @param array $facet
   * @return string
   */
  public function getFacetPrettyPathsAlias(array $facet) {
    $facet_settings = $this->adapter->getFacetSettingsGlobal($facet);
    // Path alias defaults to facet_name.
    return !empty($facet_settings->settings['pretty_paths_alias']) ?
      $facet_settings->settings['pretty_paths_alias'] : $facet['field alias'];
  }

  /**
   * Implements FacetapiUrlProcessorPrettyPaths::setBreadcrumb().
   */
  public function setBreadcrumb() {
    $breadcrumb = backdrop_get_breadcrumb();

    // $facets to use later to get the segment
    $facets = $this->adapter->getEnabledFacets();

    // Gets search keys and active items form the adapter.
    $keys = $this->adapter->getSearchKeys();
    $active_items = $this->adapter->getAllActiveItems();

    $item = menu_get_item();
    $last_load_func = is_array($item['load_functions']) ? end($item['load_functions']) : NULL;
    if (!empty($item['title']) && ((!$keys && $active_items) || ($keys && $last_load_func != 'menu_tail_load'))) {
      $last = end($breadcrumb);
      $this_page = l($item['title'], $item['href'], $item['localized_options']);
      if ($last != $this_page) {
        $breadcrumb[] = $this_page;
      }
    }

    // Initializes base breadcrumb query.
    $query = $this->params;
    unset($query[$this->filterKey]);

    // Adds the current search to the query.
    if ($keys) {
      // The last item should be text, not a link.
      $breadcrumb[] = $active_items ? l($keys, $this->getBasePath(), array('query' => $query)) : check_plain($keys);
    }

    // Use this to track the active facet trail.
    $segments = array();

    // Keep track of what the last item is.
    $last = end($active_items);

    foreach ($active_items as $item) {
      // Add this breadcrumb segment to the complete segments list.
      // Do not use aliases, use the real facet
      $segments[] = $this->getPathSegment($facets[$item['facets'][0]], $item['value']);

      // Replaces with the mapped value.
      $value = $this->adapter->getMappedValue($item['facets'][0], $item['value']);

      // The last item should be text, not a link.
      if ($last == $item) {
        $breadcrumb[] = !empty($value['#html']) ? $value['#markup'] : check_plain($value['#markup']);
      }
      else {
        $base = $this->adapter->getSearchPath();
        $path = $this->constructPath($segments);
        $breadcrumb[] = l($value['#markup'], $path, array('html' => !empty($value['#html'])));
      }
    }

    // Sets the breadcrumb trail with h keys and filters.
    backdrop_set_breadcrumb($breadcrumb);
  }

  /**
   * @return FacetapiAdapter
   */
  public function getAdapter()
  {
    return $this->adapter;
  }

  /**
   * @return null
   */
  public function getFullPath() {
    return $this->fullPath;
  }

  /**
   * @return null
   */
  public function getPathWithoutSegments() {
    return $this->pathWithoutSegments;
  }

  /**
   * Calculated the base path using the base path provider.
   *
   * @return null
   */
  public function getBasePath() {
    return $this->basePathProvider->getBasePath($this);
  }

}
