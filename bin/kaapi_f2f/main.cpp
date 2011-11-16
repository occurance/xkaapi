/*
** xkaapi
** 
**
** Copyright 2009 INRIA.
**
** Contributors :
**
** thierry.gautier@inrialpes.fr
** fabien.lementec@gmail.com / fabien.lementec@imag.fr
** 
** This software is a computer program whose purpose is to execute
** multithreaded computation with data flow synchronization between
** threads.
** 
** This software is governed by the CeCILL-C license under French law
** and abiding by the rules of distribution of free software.  You can
** use, modify and/ or redistribute the software under the terms of
** the CeCILL-C license as circulated by CEA, CNRS and INRIA at the
** following URL "http://www.cecill.info".
** 
** As a counterpart to the access to the source code and rights to
** copy, modify and redistribute granted by the license, users are
** provided only with a limited warranty and the software's author,
** the holder of the economic rights, and the successive licensors
** have only limited liability.
** 
** In this respect, the user's attention is drawn to the risks
** associated with loading, using, modifying and/or developing or
** reproducing the software by the user in light of its specific
** status of free software, that may mean that it is complicated to
** manipulate, and that also therefore means that it is reserved for
** developers and experienced professionals having in-depth computer
** knowledge. Users are therefore encouraged to load and test the
** software's suitability as regards their requirements in conditions
** enabling the security of their systems and/or data to be ensured
** and, more generally, to use and operate it in the same conditions
** as regards security.
** 
** The fact that you are presently reading this means that you have
** had knowledge of the CeCILL-C license and that you accept its
** terms.
** 
*/


#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdexcept>
#include "rose_headers.h"


class fortranDirectiveProcessing : public AstSimpleProcessing
{
private:
  static bool is_sentinel(char c)
  {
    return c == '!' || c == '*' || c == 'C';
  }

  static bool is_directive(const std::string& s)
  {
    static const char token[] = "$KAAPI";

    if (s.size() < (sizeof(token) + 1 - 1)) return false;
    if (is_sentinel(s[0]) == false) return false;
    return 0 == memcmp(s.data() + 1, token, sizeof(token) - 1);
  }

public:
  fortranDirectiveProcessing() {}

  virtual void visit(SgNode* node)
  {
    SgLocatedNode* const locatedNode = isSgLocatedNode(node);
    if (locatedNode == NULL) return ;

    AttachedPreprocessingInfoType* const comments =
      locatedNode->getAttachedPreprocessingInfo();
    if (comments == NULL) return ;

    AttachedPreprocessingInfoType::iterator pos = comments->begin();
    AttachedPreprocessingInfoType::iterator end = comments->end();
    for (; pos != end; ++pos)
    {
      const std::string& s = (*pos)->getString();
      if (is_directive(s) == false) continue ;

      printf("new_directive: %s\n", (*pos)->getString().c_str());
    }
  }
};



int main(int argc, char **argv) 
{
  SgProject* project = frontend(argc, argv);
  project->set_Fortran_only(true);
  fortranDirectiveProcessing().traverse(project, preorder);
  backend(project);
  return 0;
}
