#include "generator.ih"

void Generator::errExisting(string const &fileName, string const &option,
                             string const &pattern) const
{
    if (not d_options.specified(option))
        return;

    if (not grep(fileName, pattern))
        emsg << '`' << fileName << "' exists, option/directive `" << 
                option << "' specification conflict" << endl;
}