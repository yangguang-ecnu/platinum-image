//////////////////////////////////////////////////////////////////////////
//
//  Listedfactory $Revision: $
//
//  Object factory class used to keep track of interchangeable objects
//  of one particular class, like renderers or transfer functions
//
//  Adapted from an article by Robert Geiman
//  http://www.gamedev.net/reference/articles/article2097.asp
//
//  $LastChangedBy: arvid.r $

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <FL/Fl_Menu.h>

typedef std::string factoryIdType;

//The syntax for using this final implementation of the object factory looks like this:
//
//ObjectFactory<Shape *(int), std::string> shape_factory;
//
//shape_factory.Register<Triangle>("triangle");
//shape_factory.Register<Square>("square");
//
//
//Shape *shape1 = shape_factory.Create("triangle", 10);
//Shape *shape2 = shape_factory.Create("square", 20);

template<typename BaseClassType, typename ClassType>
BaseClassType *CreateObject()
{
    return new ClassType;
}

template<typename BaseClassType>
class listedfactory
{
protected:
   typedef BaseClassType *(*CreateObjectFunc)();
   
protected:
   std::map<factoryIdType, CreateObjectFunc> m_object_creator;

public:
   typedef typename std::map<factoryIdType, CreateObjectFunc>::const_iterator ConstIterator;
   typedef typename std::map<factoryIdType, CreateObjectFunc>::iterator Iterator;

   template<typename ClassType>
   bool Register(factoryIdType unique_id)
   {
      if (m_object_creator.find(unique_id) != m_object_creator.end())
               return false;

      m_object_creator[unique_id] = &CreateObject<BaseClassType, ClassType>;

      return true;
   }

   bool Unregister(factoryIdType unique_id)
   {
      return (m_object_creator.erase(unique_id) == 1);
   }

   BaseClassType *Create(factoryIdType unique_id)
   {
      Iterator iter = m_object_creator.find(unique_id);

      if (iter == m_object_creator.end())
         return NULL;

      return ((*iter).second)();
   }

   ConstIterator GetBegin() const
   {
      return m_object_creator.begin();
   }

   Iterator GetBegin()
   {
      return m_object_creator.begin();
   }

   ConstIterator GetEnd() const
   {
      return m_object_creator.end();
   }

   Iterator GetEnd()
   {
      return m_object_creator.end();
   }

   static Fl_Menu * function_menu ();
};
