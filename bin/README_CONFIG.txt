IMPORTANT

- Running noxim requires a YAML config file to be specified with the -config
option.

- A set of configuration examples can be found in the "config_example"
directory. In paritcular, see "default_config.yaml" for a commented
example.

- Note that parameters passed at command line overwrite any
corresponding values in the configuration file. However, note that a config file
is always required since default values of all parameters are not
hardcoded inside the source anymore. Further, some parameters having a
complex hierarchy (e.g. hub connections, wireless channels) would be
very difficult to pass using command line. Command line should be used
mostly to quickly change simple parameters when running multiple
tests, e.g. different packet inject rates, enabling/disabling
wireless, changing the routing algorithm or traffic etc...


