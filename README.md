# Islandora Web ARChive Solution Pack

[![Build Status](https://travis-ci.org/ruebot/islandora_solution_pack_web_archive.png)](https://travis-ci.org/ruebot/islandora_solution_pack_web_archive)

## Description

Web Archive Solution Pack for Islandora

Adds all required Fedora objects to allow users to ingest and retrieve web archives through the Islandora interface

## Installation

1. `cd $ISLANDORA_SITE/sites/all/modules && git clone https://github.com/ruebot/islandora_solution_pack_web_archive.git `
2. Enable module: admin/build/modules

## Solr indexing

If you are using Solr 4+, warcs will automatically be indexed via Apache Tika.

## License

GPLv3

## Todo

- [x] Display TN
- [x] Display PNG
- [x] Download link for warc
- [x] Download link for PDF
- [x] Solr integration (indexing warcs)
- [ ] Wayback Machine dissemination
- [ ] Automatic harvesting
