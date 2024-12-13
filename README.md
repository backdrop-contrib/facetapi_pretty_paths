# Facet API Pretty Paths

Enables pretty paths for searches with Facet API. You will likely use this
module in combination with either Search API or Apache Solr Search Integration.

## Installation

- Install this module using [the official Backdrop CMS instructions](https://docs.backdropcms.org/documentation/extend-with-modules).

## Usage

* Pretty paths will be generated as `search/url/segment1/segment2/`.
* By default, a segment will look like: `<alias>/<value>`.
* Custom coder plugins, allow to override the default segment representation.
* The taxonomy coder outputs the id: `<alias>/<term-name>-<term-id>`.
* The taxonomy pathauto coder leverages pathauto aliases to create even prettier paths: `<alias>/<term-alias>`.
* Pager & breadcrumbs supported.
* Supports the optional Current Search Blocks "current_search" module of Facet API.

## Issues

Bugs and feature requests should be reported in [the issue queue](https://github.com/backdrop-contrib/facetapi_pretty_paths/issues).

## Current Maintainers

- [Herb v/d Dool](https://github.com/herbdool)
- Seeking additional maintainer(s).

## Credits

- Ported to Backdrop CMS by [Herb v/d Dool](https://github.com/herbdool).
- Originally written for Drupal by [dasjo](https://www.drupal.org/u/dasjo), [nick_vh](https://www.drupal.org/u/nick_vh), [voidberg](https://www.drupal.org/u/voidberg).

## License

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
