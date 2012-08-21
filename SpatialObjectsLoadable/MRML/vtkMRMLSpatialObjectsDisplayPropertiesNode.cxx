/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Michael Jeulin-L, Kitware Inc.

==============================================================================*/

#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkMRMLSpatialObjectsDisplayPropertiesNode.h"

#include <vtkLineSource.h>
#include <vtkTubeFilter.h>
#include <vtkSphereSource.h>

vtkCxxSetObjectMacro(vtkMRMLSpatialObjectsDisplayPropertiesNode,
                     GlyphSource,
                     vtkPolyData);

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLSpatialObjectsDisplayPropertiesNode);


//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsDisplayPropertiesNode::
vtkMRMLSpatialObjectsDisplayPropertiesNode()
{
  // Default display
  this->ScalarInvariant = this->LinearMeasure;
  this->GlyphGeometry = this->Cones;
  this->ColorGlyphBy = this->LinearMeasure;

  // Glyph general parameters
  this->GlyphScaleFactor = 50;

  // Line Glyph parameters
  this->LineGlyphResolution = 20;

  // Tube Glyph parameters
  this->TubeGlyphRadius = 0.1;
  this->TubeGlyphNumberOfSides = 6;

  // VTK Objects
  this->GlyphSource = NULL;
  this->UpdateGlyphSource();

  // set the type to user
  this->SetTypeToUser();
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsDisplayPropertiesNode::
~vtkMRMLSpatialObjectsDisplayPropertiesNode()
{
  if ( this->GlyphSource != NULL )
    {
    this->GlyphSource->Delete();
    }
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayPropertiesNode::WriteXML(ostream& oss,
                                                          int nIndent)
{
  Superclass::WriteXML(oss, nIndent);
  
  vtkIndent indent(nIndent);
  oss << indent << " glyphGeometry=\""
      << this->GlyphGeometry << "\"";
  oss << indent << " colorGlyphBy=\""
      << this->ColorGlyphBy << "\"";
  oss << indent << " glyphScaleFactor=\""
      << this->GlyphScaleFactor << "\"";
  oss << indent << " lineGlyphResolution=\""
      << this->LineGlyphResolution << "\"";
  oss << indent << " tubeGlyphRadius=\""
      << this->TubeGlyphRadius << "\"";
  oss << indent << " tubeGlyphNumberOfSides=\""
      << this->TubeGlyphNumberOfSides << "\"";
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayPropertiesNode::
ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
  {
      attName = *(atts++);
      attValue = *(atts++);
      if (!strcmp(attName, "glyphGeometry")) 
      {
      int glyphGeometry;
      std::stringstream ss;
      ss << attValue;
      ss >> glyphGeometry;
      this->SetGlyphGeometry(glyphGeometry);
      }
      else if (!strcmp(attName, "colorGlyphBy")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> ColorGlyphBy;
      }
      else if (!strcmp(attName, "glyphScaleFactor")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> GlyphScaleFactor;
      }
      else if (!strcmp(attName, "lineGlyphResolution")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> LineGlyphResolution;
      }
      else if (!strcmp(attName, "tubeGlyphRadius")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> TubeGlyphRadius;
      }
      else if (!strcmp(attName, "tubeGlyphNumberOfSides")) 
      {
      std::stringstream ss;
      ss << attValue;
      ss >> TubeGlyphNumberOfSides;
      }
  }
  this->EndModify(disabledModify);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayPropertiesNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLSpatialObjectsDisplayPropertiesNode *node =
    vtkMRMLSpatialObjectsDisplayPropertiesNode::SafeDownCast(anode);

  this->SetScalarInvariant(node->ScalarInvariant);
  this->SetGlyphGeometry(node->GlyphGeometry);
  this->SetColorGlyphBy(node->ColorGlyphBy);
  this->SetGlyphScaleFactor(node->GlyphScaleFactor);
  this->SetLineGlyphResolution(node->LineGlyphResolution);
  this->SetTubeGlyphRadius(node->TubeGlyphRadius);
  this->SetTubeGlyphNumberOfSides(node->TubeGlyphNumberOfSides);

  this->EndModify(disabledModify);
  }

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayPropertiesNode::PrintSelf(ostream& os,
                                                           vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
  os << indent << "ScalarInvariant "
     << this->ScalarInvariant << "\n";
  os << indent << "GlyphGeometry: "
     << this->GlyphGeometry << "\n";
  os << indent << "ColorGlyphBy: "
     << this->ColorGlyphBy << "\n";
  os << indent << "GlyphScaleFactor: "
     << this->GlyphScaleFactor << "\n";
  os << indent << "LineGlyphResolution: "
     << this->LineGlyphResolution << "\n";
  os << indent << "TubeGlyphRadius: "
     << this->TubeGlyphRadius << "\n";
  os << indent << "TubeGlyphNumberOfSides: "
     << this->TubeGlyphNumberOfSides << "\n";
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayPropertiesNode::SetGlyphGeometry(int geometry)
{
  if ( this->GlyphGeometry != geometry )
    {
    this->GlyphGeometry = geometry;
    this->UpdateGlyphSource();
    this->Modified();
    }
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayPropertiesNode::UpdateGlyphSource()
{
  vtkDebugMacro("Get Glyph Source");

  // Get rid of any old glyph source
  if ( this->GlyphSource != NULL )
    {
    this->GlyphSource->Delete();
    this->GlyphSource = NULL;
    }
  
  // Create a new glyph source according to current settings
  switch (this->GlyphGeometry)
    {
    case Lines:
    case Tubes:
      {
      vtkLineSource *line = vtkLineSource::New();
      line->SetResolution(this->LineGlyphResolution);
      line->Update();

      // if we are doing tubes, put a tube on the line
      if (this->GlyphGeometry == Tubes)
        {
        vtkTubeFilter *tube = vtkTubeFilter::New();
        tube->SetInput(line->GetOutput());
        tube->SetRadius( this->TubeGlyphRadius );
        tube->SetNumberOfSides( this->TubeGlyphNumberOfSides );
        tube->Update();

        this->SetGlyphSource(tube->GetOutput());
        tube->Delete();

        vtkDebugMacro("Get Glyph Source: Tubes");
        }
      else
        {
        vtkDebugMacro("Get Glyph Source: Lines");
        this->SetGlyphSource(line->GetOutput());
        }
      line->Delete( );
      }
      break;
    }
}

//------------------------------------------------------------------------------
int vtkMRMLSpatialObjectsDisplayPropertiesNode::GetFirstScalarInvariant()
{
  return vtkMRMLSpatialObjectsDisplayPropertiesNode::LinearMeasure;
}

//------------------------------------------------------------------------------
int vtkMRMLSpatialObjectsDisplayPropertiesNode::GetLastScalarInvariant()
{
  return vtkMRMLSpatialObjectsDisplayPropertiesNode::RelativeAnisotropy;
}

//------------------------------------------------------------------------------
const char* vtkMRMLSpatialObjectsDisplayPropertiesNode::
GetScalarEnumAsString(int var)
{
  if (var == vtkMRMLSpatialObjectsDisplayPropertiesNode::RelativeAnisotropy)
    {
    return "RelativeAnisotropy";
    }
  if (var == vtkMRMLSpatialObjectsDisplayPropertiesNode::LinearMeasure)
    {
    return "LinearMeasure";
    }
  if (var == vtkMRMLSpatialObjectsDisplayPropertiesNode::ColorOrientation)
    {
    return "ColorOrientation";
    }
  if (var == vtkMRMLSpatialObjectsDisplayPropertiesNode::ColorMode)
    {
    return "ColorMode";
    }
  return "(unknown)";
}

//------------------------------------------------------------------------------
const char* vtkMRMLSpatialObjectsDisplayPropertiesNode::
GetScalarInvariantAsString()
{
  return this->GetScalarEnumAsString(this->ScalarInvariant);
}

//------------------------------------------------------------------------------
const char* vtkMRMLSpatialObjectsDisplayPropertiesNode::
GetGlyphGeometryAsString()
{
  return this->GetGlyphGeometryAsString(this->GlyphGeometry);
}

//------------------------------------------------------------------------------
const char* vtkMRMLSpatialObjectsDisplayPropertiesNode::
GetGlyphGeometryAsString(int geometry)
{
  if (geometry == this->Lines)
    {
    return "Lines";
    }
  if (geometry == this->Tubes)
    {
    return "Tubes";
    }
  return "(unknown)";
}

//------------------------------------------------------------------------------
const char* vtkMRMLSpatialObjectsDisplayPropertiesNode::
GetColorGlyphByAsString()
{
  return this->GetScalarEnumAsString(this->ColorGlyphBy);
}

//------------------------------------------------------------------------------
int vtkMRMLSpatialObjectsDisplayPropertiesNode::GetFirstColorGlyphBy()
{
  return vtkMRMLSpatialObjectsDisplayPropertiesNode::LinearMeasure;
}

//------------------------------------------------------------------------------
int vtkMRMLSpatialObjectsDisplayPropertiesNode::GetLastColorGlyphBy()
{
  return vtkMRMLSpatialObjectsDisplayPropertiesNode::RelativeAnisotropy;
}

//------------------------------------------------------------------------------
bool vtkMRMLSpatialObjectsDisplayPropertiesNode::
ScalarInvariantHasKnownScalarRange(int ScalarInvariant)
{
  switch (ScalarInvariant)
    {
    case vtkMRMLSpatialObjectsDisplayPropertiesNode::ColorOrientation:
    case vtkMRMLSpatialObjectsDisplayPropertiesNode::ColorMode:
    case vtkMRMLSpatialObjectsDisplayPropertiesNode::LinearMeasure:
      {
      return 1;
      break;
      }
    case vtkMRMLSpatialObjectsDisplayPropertiesNode::RelativeAnisotropy:
      {
        return 0;
        break;
      }
    default:
      return 0;
      break;
    }
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayPropertiesNode
::ScalarInvariantKnownScalarRange(int ScalarInvariant, double range[2])
{
  switch (ScalarInvariant)
    {
    case vtkMRMLSpatialObjectsDisplayPropertiesNode::ColorOrientation:
    case vtkMRMLSpatialObjectsDisplayPropertiesNode::ColorMode:
      {
      range[0] = 0;
      range[1] = 255.;
      break;
      }
    case vtkMRMLSpatialObjectsDisplayPropertiesNode::RelativeAnisotropy:
    case vtkMRMLSpatialObjectsDisplayPropertiesNode::LinearMeasure:
      {
      range[0] = 0;
      range[1] = 1.;
      break;
      }
    }
}
