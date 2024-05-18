using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RideWithMe.BL.Models.Address;
using System.ComponentModel.DataAnnotations;
using RideWithMe.BL.Models;
using RideWithMe.Common.Enums;

namespace RideWithMe.App.Wrappers
{
    public class AddressWrapper  : ModelWrapper<AddressDetailModel>
    {
        public AddressWrapper(AddressDetailModel? model) : base(model)
        {
        }


        public string? State
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        
        public string? Street
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        
        public string? City
        {
            get => GetValue<string>();
            set => SetValue(value);
        }

        public int? HouseNumber
        {
            get => GetValue<int>();
            set => SetValue(value);
        }
        
        public override IEnumerable<ValidationResult> Validate(ValidationContext validationContext)
        {
            if (HouseNumber <= 0)
            {
                yield return new ValidationResult($"{nameof(HouseNumber)} is required", new[] { nameof(HouseNumber) });
            }

            if (String.IsNullOrEmpty(State))
            {
                yield return new ValidationResult($"{nameof(Street)} is required", new[] { nameof(Street) });
            }
            
            if (String.IsNullOrEmpty(Street))
            {
                yield return new ValidationResult($"{nameof(Street)} is required", new[] { nameof(Street) });
            }
            
            if (String.IsNullOrEmpty(City))
            {
                yield return new ValidationResult($"{nameof(City)} is required", new[] { nameof(City) });
            }
        }


        public static implicit operator AddressWrapper(AddressDetailModel addressDetailModel) => new(addressDetailModel);

        public static implicit operator AddressDetailModel(AddressWrapper wrapper) => wrapper.Model;

    }
}
