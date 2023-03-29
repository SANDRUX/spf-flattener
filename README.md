# SPF-Flattener

SPF-Flattener is a command-line tool written in C++ that takes a domain as an input and outputs the flattened IP addresses according to the given input. The tool uses the SPF (Sender Policy Framework) DNS record of the domain to obtain the information about the authorized mail servers for that domain and then flattens the resulting IP addresses to make them easier to read and process.

## Requirements

SPF-Flattener requires CMake to be installed on your system in order to build the project. You can download CMake from their official website: https://cmake.org/download/

## Building

To build the SPF-Flattener project, follow these steps:

1. Clone the repository to your local machine.
2. Navigate to the root directory of the project.
3. Run `cmake .` to generate the build files.
4. Run `make` to build the project.

## Usage

To use the SPF-Flattener tool, follow these steps:

1. Open a terminal or command prompt.
2. Navigate to the directory where the `spf` executable is located.
3. Run `./spf <domain>` where `<domain>` is the domain you want to obtain the flattened IP addresses for.

Example usage: `./spf example.com`

## License

SPF-Flattener is licensed under the MIT license. See the LICENSE file for more details.

## Contributions

Contributions to SPF-Flattener are welcome! If you have any bug reports, feature requests, or pull requests, please submit them through the GitHub repository: https://github.com/SANDRUX/spf-flattener